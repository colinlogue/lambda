//
// Created by colin on 6/2/20.
//

#ifndef LAMBDA_EVAL_H
#define LAMBDA_EVAL_H

#include <string>
#include <lang_tools/eval/eval.hpp>

#include "result/Result.hpp"

#include "parse.h"

namespace lambda
{
    using Context = lang_tools::Context<Term>;

    using Value = Abstraction;

    using EvalResult = result::Result<Value, lang_tools::EvalErr>;

    auto reduce(const Term& term) -> Term;

    auto evaluate(const Term& term, const Context& context) -> EvalResult;
};


#endif //LAMBDA_EVAL_H
