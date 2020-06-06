//
// Created by colin on 6/5/20.
//

#include "helpers.h"

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