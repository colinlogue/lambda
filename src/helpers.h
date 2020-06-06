//
// Created by colin on 6/3/20.
//

#ifndef LAMBDA_HELPERS_H
#define LAMBDA_HELPERS_H

#include "eval.h"

namespace lambda
{
    auto app(Term lhs, Term rhs) -> Term;

    auto var(std::string name) -> Term;

    auto lam(std::string name, Term body) -> Term;
}
#endif //LAMBDA_HELPERS_H
