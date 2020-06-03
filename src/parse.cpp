//
// Created by colin on 6/2/20.
//
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

        // if nothing left, return just the atom
        if (tokens.empty())
            return left_result;

        // otherwise apply the atom to the remaining tokens
        ParseResult right_result {parse_term(tokens)};

        // if error, return the error
        if (right_result.is_err())
            return right_result;

        // otherwise both left and right are ok
        Term& left {*left_result.get_ok()};
        Term& right {*right_result.get_ok()};
        return ParseResult::make_ok(Application(left, right));
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
            Term& subterm {*(name_result.get_ok())};

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

}