//
// Created by colin on 6/2/20.
//

#ifndef LAMBDA_PARSE_H
#define LAMBDA_PARSE_H

#include <queue>
#include <string>
#include <memory>

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

    struct Variable {
        const std::string name;
    };

    struct Application {
        Application(const Term& lhs, const Term& rhs)
            : lhs {std::make_shared<Term>(lhs)}
            , rhs {std::make_shared<Term>(rhs)} {}
        const term_ptr lhs;
        const term_ptr rhs;
    };

    struct Abstraction {
        Abstraction(const Variable& name, const Term& body)
            : name {name}, body {std::make_shared<Term>(body)} {}
        const Variable name;
        const term_ptr body;
    };


    using lang_tools::ParseErr;

    using ParseResult = lang_tools::ParseResult<Term>;

    auto parse(std::queue<Token> tokens) -> ParseResult;
}

#endif //LAMBDA_PARSE_H
