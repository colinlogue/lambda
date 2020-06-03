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

namespace lambda
{
    const lang_tools::Context<Term> prelude {
        {"true",  Abstraction {"t", Abstraction { "f", "t"}}},
        {"false", Abstraction {"t", Abstraction {"f", "f"}}},
        {"zero",  Abstraction {"s", Abstraction {"z", "z"}}},
        {"succ",  Abstraction {"n", Abstraction {"s", Abstraction {"z", Application {"s", Application {Application {"n", "s"}, "z"}}}}}},
        {"plus",  Abstraction {"m", Abstraction {"n", Abstraction {"s", Abstraction {"z", Application {Application {"m", "s"}, Application { Application {"n", "s"}, "z"}}}}}}},
    };
}

#endif //LAMBDA_PRELUDE_H
