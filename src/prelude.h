//
// Created by colin on 6/3/20.
//

/**
 * This file contains a library of useful built-in definitions that are
 * available in the REPL by default.
 */

#ifndef LAMBDA_PRELUDE_H
#define LAMBDA_PRELUDE_H

#include "lang_tools/eval/eval.hpp"

#include "parse.h"
#include "helpers.h"

namespace lambda
{
    auto get_prelude() -> lang_tools::Context<Term>;


}

#endif //LAMBDA_PRELUDE_H
