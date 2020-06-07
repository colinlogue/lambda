//
// Created by colin on 6/2/20.
//

#ifndef LAMBDA_PARSE_H
#define LAMBDA_PARSE_H

#include <fstream>
#include <queue>
#include <string>
#include <memory>
#include <utility>

#include "lang_tools/parse/parse.hpp"
#include "lang_tools/eval/eval.hpp"
#include "lex.h"
#include "result/Result.hpp"

namespace lambda
{
    struct Variable;
    struct Application;
    struct Abstraction;

    using Term = std::variant<Variable, Application, Abstraction>;
    using term_ptr = std::shared_ptr<Term>;

    auto compare(const Term& lhs, const Term& rhs) -> bool;

    struct Variable {
        Variable(const char* name) : name {name} {}
        Variable(std::string name) : name {std::move(name)} {}
        std::string name;

        auto operator ==(const Variable& other) const -> bool
        {
            return name == other.name;
        }
    };

    struct Application {
        Application(const Term& lhs, const Term& rhs)
            : lhs {std::make_shared<Term>(lhs)}
            , rhs {std::make_shared<Term>(rhs)} {}
        term_ptr lhs;
        term_ptr rhs;

        auto operator ==(const Application& other) const -> bool;

    };

    struct Abstraction {
        Abstraction(Variable name, const Term& body)
            : name {std::move(name)}, body {std::make_shared<Term>(body)} {}
        Abstraction(Variable name, Term&& body)
            : name {std::move(name)}, body {std::make_shared<Term>(body)} {}
        Variable name;
        term_ptr body;

        auto operator ==(const Abstraction& other) const -> bool
        {
            return name == other.name && compare(*body, *other.body);
        }
    };


    using Substitution = std::pair<std::string, Term>;
    using lang_tools::ParseErr;

    using ParseResult = lang_tools::ParseResult<Term>;

    auto parse(std::queue<Token> tokens) -> ParseResult;

    auto substitute(Substitution sub, const Term& term) -> Term;

    auto as_string(const Term& term) -> std::string;

    auto operator <<(std::ostream& out, const Term& term) -> std::ostream&;

    auto parse_string(std::string str) -> std::optional<Term>;

    auto parse_file(std::fstream& file) -> lang_tools::ParseResult<lang_tools::Context<Term>>;
}

#endif //LAMBDA_PARSE_H
