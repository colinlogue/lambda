//
// Created by colin on 6/2/20.
//

#ifndef LANG_TOOLS_EVAL_HPP
#define LANG_TOOLS_EVAL_HPP

#include <string>

#include "result/Result.hpp"

namespace lang_tools
{
    using EvalErr = std::string;

    template <typename Value>
    using EvalResult = result::Result<Value, EvalErr>;

    template <typename Term>
    using Context = std::unordered_map<std::string, Term>;

    template <typename Value, typename Term>
    using Evaluator = std::function<EvalResult<Value>(Term, Context<Term>)>;
}

#endif //LANG_TOOLS_EVAL_HPP
