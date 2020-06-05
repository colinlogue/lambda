//
// Created by colin on 6/2/20.
//
#include <sstream>
#include <utility>
#include <variant>

#include "lang_tools/parse/parse.hpp"
#include "result/Result.hpp"
#include "parse.h"


//      Grammar
//      -------
//    Term ->	Abst
//          |	Appl
//    Abst ->	\ Name . Term
//    Appl ->	Atom
//          |	Appl Atom
//    Atom ->	( Term )
//          |	Name
//    Name →	[a-zA-Z]+
//

namespace lambda
{
    auto parse_term(std::queue<Token>& tokens) -> ParseResult;

    auto unexpected_token(TokenType expected, TokenType got) -> ParseResult
    {
        return ParseResult::make_err(
                "expected " + as_string(expected) + ", got " + as_string(got));
    }

    auto parse_name(std::queue<Token>& tokens) -> ParseResult
    {
        Token tok {tokens.front()};
        tokens.pop();

        // if token is name return ok
        if (tok.type == TokenType::Name)
            return ParseResult::make_ok(Variable { tok.value });

        // otherwise err
        return unexpected_token(TokenType::Name, tok.type);
    }

    auto parse_atom(std::queue<Token>& tokens) -> ParseResult
    {
        if (tokens.front().type == TokenType::LeftParen)
        {
            tokens.pop();
            ParseResult term_result {parse_term(tokens)};

            // check matching closing paren
            if (tokens.front().type == TokenType::RightParen)
            {
                tokens.pop();
                return std::move(term_result);
            }
            else
            {
                return unexpected_token(TokenType::RightParen, tokens.front().type);
            }
        }
        else
        {
            return parse_name(tokens);
        }
    }

    auto parse_application(std::queue<Token>& tokens) -> ParseResult
    {
        ParseResult left_result {parse_atom(tokens)};

        // if it was an error, return early
        if (left_result.is_err())
            return left_result;

        // if nothing left or closing paren, return just the atom
        if (tokens.empty() || tokens.front().type == TokenType::RightParen)
            return left_result;

        // otherwise apply the atom to the remaining tokens
        ParseResult right_result {parse_atom(tokens)};

        // if error, return the error
        if (right_result.is_err())
            return right_result;

        // otherwise both left and right are ok
        Term& left {*left_result.get_ok()};
        Term& right {*right_result.get_ok()};
        Term app {Application {left, right}};

        while (!tokens.empty() && tokens.front().type != TokenType::RightParen)
        {
            // otherwise apply this application to remainder to get a new application
            ParseResult rem_result{parse_atom(tokens)};
            if (rem_result.is_err())
                return rem_result;

            app = Application(app, *rem_result.get_ok());
        }

        return ParseResult::make_ok(app);
    }

    auto parse_abstraction(std::queue<Token>& tokens) -> ParseResult
    {
        if (tokens.front().type == TokenType::Lambda)
        {
            tokens.pop();
            ParseResult name_result {parse_name(tokens)};

            if (name_result.is_err())
                return name_result;

            // safe to use get directly because parse_name only returns Variable
            Variable name {std::get<Variable>(*name_result.get_ok())};

            if (tokens.front().type != TokenType::Dot)
                return unexpected_token(TokenType::Dot, tokens.front().type);

            tokens.pop();
            ParseResult subterm_result {parse_abstraction(tokens)};

            // return early if there was an error
            if (subterm_result.is_err())
                return subterm_result;

            // safe to dereference because we just checked if it was an error
            Term& subterm {*(subterm_result.get_ok())};

            return ParseResult::make_ok(Abstraction(name, subterm));
        }
        else
        {
            return parse_application(tokens);
        }
    }

    auto parse_term(std::queue<Token>& tokens) -> ParseResult
    {
        return parse_abstraction(tokens);
    }

    auto parse(std::queue<Token> tokens) -> ParseResult
    {
        ParseResult result {parse_abstraction(tokens)};

        // parse is only successful if it used all tokens
        if (tokens.empty())
            return result;

        // otherwise return error
        return ParseResult::make_err("Tokens remaining after parsing");

    }

    class TermPrinter
    {
    public:
        TermPrinter() = default;

        auto operator ()(Variable var) -> std::string
        {
            return var.name;
        }

        auto operator ()(Abstraction abstr) -> std::string
        {
            std::stringstream str;
            str << "\\" << abstr.name << "." << *abstr.body;
            return str.str();
        }

        auto operator ()(Application appl) -> std::string
        {
            std::stringstream str;
            str << *appl.lhs << " " << *appl.rhs;
            return str.str();
        }
    };

    auto as_string(const Term& term) -> std::string
    {
        return std::visit(TermPrinter {}, term);
    }

    auto operator <<(std::ostream& out, const Term& term) -> std::ostream&
    {
        out << as_string(term);
        return out;
    }

    auto rename(std::string orig) -> std::string
    {
        return orig + "`";
    }

    class Substitutor
    {
    public:

        Substitutor() = delete;
        Substitutor(Substitution sub) : sub {std::move(sub)} {}

        auto operator ()(Variable var)      -> Term;
        auto operator ()(Abstraction abstr) -> Term;
        auto operator ()(Application appl)  -> Term;

    private:
        Substitution sub;
    };

    auto Substitutor::operator()(Variable var) -> Term
    {
        if (var.name == sub.first)
            return sub.second;

        return var;
    }

    auto Substitutor::operator()(Abstraction abstr) -> Term
    {
        // if name conflict, rename bound variable terms and call again
        if (abstr.name.name == sub.first)
        {
            std::string new_name {rename(abstr.name.name)};
            Substitution sub2 {abstr.name.name, Variable {new_name}};
            return substitute(sub, Abstraction {new_name, substitute(sub2, *abstr.body)});
        }
        return Abstraction {abstr.name, substitute(sub, *abstr.body)};
    }

    auto Substitutor::operator()(Application appl) -> Term
    {
        // sub each side
        return Application {substitute(sub, *appl.lhs), substitute(sub, *appl.rhs)};
    }

    auto substitute(Substitution sub, const Term& term) -> Term
    {
        return std::visit(Substitutor(std::move(sub)), term);
    }

    auto compare(const Term& lhs, const Term& rhs) -> bool
    {
        auto lhs_as_var {std::get_if<Variable>(&lhs)};
        auto rhs_as_var {std::get_if<Variable>(&rhs)};
        if (lhs_as_var && rhs_as_var)
            return *lhs_as_var == *rhs_as_var;

        auto lhs_as_abstr {std::get_if<Abstraction>(&lhs)};
        auto rhs_as_abstr {std::get_if<Abstraction>(&rhs)};
        if (lhs_as_abstr && rhs_as_abstr)
            return *lhs_as_abstr == *rhs_as_abstr;

        auto lhs_as_appl {std::get_if<Application>(&lhs)};
        auto rhs_as_appl {std::get_if<Application>(&rhs)};
        if (lhs_as_appl && rhs_as_appl)
            return *lhs_as_appl == *rhs_as_appl;

        return false;
    }

    auto Application::operator ==(const Application& other) const -> bool
    {
        return compare(*lhs, *other.lhs) && compare(*rhs, *other.rhs);
    }

    auto parse_string(std::string str) -> std::optional<Term>
    {
        std::stringstream stream {str};
        auto lex_result {read(stream)};
        std::queue<Token> tokens;
        for (auto token : lex_result)
        {
            tokens.push(token);
        }
        auto parse_result {parse(tokens)};
        Term* term {parse_result.get_ok()};
        if (term) return *term;
        return {};
    }
}