//
// Created by colin on 6/2/20.
//

#include "parse.h"
#include "eval.h"

namespace lambda
{
    auto evaluate(Term term, std::unordered_map<std::string, Term> context)
           -> result::Result<Value, lang_tools::EvalErr>
    {
        return EvalResult::make_err("evaluation not implemented yet");
    }
}