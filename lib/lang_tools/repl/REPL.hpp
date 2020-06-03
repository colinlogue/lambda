//
// Created by colin on 5/22/20.
//

#ifndef LANG_TOOLS_REPL_HPP
#define LANG_TOOLS_REPL_HPP

#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <stack>
#include <unordered_map>
#include <queue>

#include "lang_tools/eval/eval.hpp"
#include "lang_tools/lexer/lexer.hpp"
#include "lang_tools/parse/parse.hpp"
#include "result/Result.hpp"

namespace lang_tools
{
    template <typename Token, typename Term, typename Value>
    class REPL {
    public:

        // constructors
        REPL() = delete;
        REPL(Lexer<Token> lexer,
                Parser<Term, Token> parser,
                Evaluator<Value, Term> evaluator);

        struct state_t
        {
            state_t() = default;

            bool exit_flag {false};
        };

        class Command
        {
        public:
            enum class Action {Exit, Print};
            using op_type = std::function<std::optional<std::string>(state_t&, const std::string&)>;

            explicit Command(op_type operation);

            inline auto run(state_t& state, const std::string& buffer) -> std::optional<std::string>;

        private:
            const op_type operation;
        };

        auto run() -> void;

        auto set_prompt(std::string new_prompt) -> REPL&;
        auto set_welcome(std::string new_welcome) -> REPL&;
        auto set_goodbye(std::string new_goodbye) -> REPL&;

    private:
        // in and out streams
        std::ostream& out {std::cout};
        std::istream& in {std::cin};

        // components
        Lexer<Token> lexer;
        Parser<Term, Token> parser;
        Evaluator<Value, Term> evaluator;

        // context
        std::unordered_map<std::string, Term> context {};

        // messages
        std::string prompt {"> "};
        std::string welcome_msg {"Welcome"};
        std::string goodbye_msg {"Goodbye"};

        // commands
        // TODO: better way to implement commands
        Command cmd_exit
                {
                    [](state_t& s, std::string b) -> std::optional<std::string>
                    {
                        s.exit_flag = true;
                        return {};
                    }
                };

        Command cmd_help
                {
                    [](state_t& s, std::string b) -> std::optional<std::string>
                    {
                        return {"help text here"};
                    }
                };

        std::unordered_map<std::string, Command> commands
                {
                    {"exit", cmd_exit},
                    {"help", cmd_help},
                };

        // state
        state_t state {};

        // loop
        auto loop(std::string& buffer, std::stringstream& stream) -> void;

        // actions
        auto welcome() const -> void;
        auto goodbye() const -> void;
        auto exit() -> void;

        struct token_stream
        {
            REPL& owner;
            std::istream& in;

            explicit token_stream(REPL& owner, std::istream& in)
                : owner { owner }, in { in } {}

            auto begin() -> token_iterator<Token>
            {
                return token_iterator<Token>(owner.lexer, in);
            }

            auto end() -> token_iterator<Token>
            {
                return token_iterator<Token>();
            }
        };

    };

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::run() -> void
    {
        // initialize buffer and stream
        std::string buffer;
        std::stringstream stream;

        // print starting message
        welcome();

        // enter loop
        // loop will exit on its own, so just
        // handle any exceptions here
        try
        {
            while (!(state.exit_flag))
            {
                loop(buffer, stream);
            }
        }
        catch (std::exception& e)
        {
            out << "Terminating after uncaught exception: "
                << e.what()
                << std::endl;
        }
        catch (...)
        {
            out << "Terminating after unknown error"
                << std::endl;
        }
    }

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            welcome() const -> void
    {
        out << welcome_msg << std::endl;
    }

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            set_prompt(std::string new_prompt) -> REPL &
    {
        prompt = std::move(new_prompt);
        return *this;
    }

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            set_goodbye(std::string new_goodbye) -> REPL &
    {
        goodbye_msg = std::move(new_goodbye);
        return *this;
    }

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            set_welcome(std::string new_welcome) -> REPL &
    {
        welcome_msg = std::move(new_welcome);
        return *this;
    }

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            exit() -> void
    {
        out << goodbye_msg
            << std::endl;
    }

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            goodbye() const -> void
    {
        out << goodbye_msg << std::endl;
    }

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            loop(std::string& buffer, std::stringstream& stream) -> void
    {
        // display prompt
        out << prompt;

        // get input
        std::getline(in, buffer);

        // check commands
        auto cmd{commands.find(buffer)};

        // run command if input was a match
        if (cmd != commands.end())
        {
            std::optional<std::string> maybe_message {(cmd->second).run(state, buffer)};
            if (maybe_message.has_value())
                out << maybe_message.value() << std::endl;
        }
        else // input was not a command
        {
            // clear stream and update
            stream.clear();
            stream.str(buffer);

            // get tokens
            std::queue<Token> tokens;
            for (LexResult<Token>& result : token_stream(*this, stream))
            {
                Token* ok {result.get_ok()};
                if (ok != nullptr)
                    tokens.push(*ok);
                else
                {
                    LexErr* err {result.get_err()};
                    out << "lex error: " << *err << std::endl;
                    return;
                }
            }
            // if loop finished, then no errors occurred
            // otherwise returned early

            // get term
            ParseResult<Term> parse_result {parser(tokens)};
            ParseErr* parse_err {parse_result.get_err()};
            if (parse_err != nullptr)
            {
                out << "parse error: " << *parse_err << std::endl;
                return;
            }

            // no parse error
            // attempt to evaluate term in context
            Term* ok {parse_result.get_ok()};
            EvalResult<Value> eval_result {evaluator(*ok, context)};

            Value* val {eval_result.get_ok()};
            if (val != nullptr)
                out << val << std::endl;
            else // evaluation error
            {
                EvalErr* eval_err {eval_result.get_err()};
                out << "evaluation error: " << eval_err << std::endl;
            }
            return;
        }
    }

    template <typename Token, typename Term, typename Value>
    REPL<Token, Term, Value>::REPL(Lexer<Token> lexer, Parser<Term, Token> parser,
            Evaluator<Value, Term> evaluator)
        : lexer {lexer}, parser {parser}, evaluator {evaluator}
    {}

    template <typename Token, typename Term, typename Value>
    REPL<Token, Term, Value>::
    Command::Command(REPL::REPL::Command::op_type operation)
        : operation { operation }
    {}

    template <typename Token, typename Term, typename Value>
    auto REPL<Token, Term, Value>::
            Command::run(REPL::state_t& state_in, const std::string& buffer) -> std::optional<std::string>
    {
        return operation(state_in, buffer);
    }

}

#endif //LANG_TOOLS_REPL_HPP
