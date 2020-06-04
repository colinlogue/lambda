//
// Created by colin on 6/3/20.
//

#ifndef LAMBDA_HELPERS_H
#define LAMBDA_HELPERS_H

#include "eval.h"

namespace lambda
{
    auto app(Term lhs, Term rhs) -> Term
    {
        return Application{lhs, rhs};
    }

    auto var(std::string name) -> Term
    {
        return Variable{std::move(name)};
    }

    auto lam(std::string name, Term body) -> Term
    {
        return Abstraction{std::move(name), body};
    }
}
#endif //LAMBDA_HELPERS_H
