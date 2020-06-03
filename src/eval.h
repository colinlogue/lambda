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
    class Value {};

    using EvalResult = result::Result<Value, lang_tools::EvalErr>;

    auto evaluate(Term term, std::unordered_map<std::string, Term> context)
            -> result::Result<Value, lang_tools::EvalErr>;
};


#endif //LAMBDA_EVAL_H
