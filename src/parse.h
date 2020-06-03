//
// Created by colin on 6/2/20.
//

#ifndef LAMBDA_PARSE_H
#define LAMBDA_PARSE_H

#include <queue>
#include <string>
#include <memory>
#include <utility>

#include "lang_tools/parse/parse.hpp"
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
        const std::string name;

        auto operator ==(const Variable& other) const -> bool
        {
            return name == other.name;
        }
    };

    struct Application {
        Application(const Term& lhs, const Term& rhs)
            : lhs {std::make_shared<Term>(lhs)}
            , rhs {std::make_shared<Term>(rhs)} {}
        const term_ptr lhs;
        const term_ptr rhs;

        auto operator ==(const Application& other) const -> bool
        {
            return compare;
        }
    };

    struct Abstraction {
        Abstraction(Variable name, const Term& body)
            : name {std::move(name)}, body {std::make_shared<Term>(body)} {}
        Abstraction(Variable name, Term&& body)
            : name {std::move(name)}, body {std::make_shared<Term>(body)} {}
        const Variable name;
        const term_ptr body;

        auto operator ==(const Variable& other) const -> bool
        {
            return name == other.name;
        }
    };

    using lang_tools::ParseErr;

    using ParseResult = lang_tools::ParseResult<Term>;

    auto parse(std::queue<Token> tokens) -> ParseResult;

    using Substitution = std::pair<std::string, Term>;

    auto substitute(Substitution sub, const Term& term) -> Term;

    auto as_string(const Term& term) -> std::string;

    auto operator <<(std::ostream& out, const Term& term) -> std::ostream&;
}

#endif //LAMBDA_PARSE_H
