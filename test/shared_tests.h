//
// Created by colin on 6/5/20.
//

#ifndef LAMBDA_SHARED_TESTS_H
#define LAMBDA_SHARED_TESTS_H

#include <string>

#include "bandit/bandit.h"
#include "eval.h"


using namespace snowhouse;
using namespace bandit;

using namespace lambda;

auto parse_test(std::string term_str, const lambda::Term& expected) -> void;

#endif //LAMBDA_SHARED_TESTS_H
