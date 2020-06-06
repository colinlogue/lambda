//
// Created by colin on 6/6/20.
//

/**
 * Representing natural numbers as Church numerals
 */

#ifndef LAMBDA_NUMERALS_H
#define LAMBDA_NUMERALS_H

#include <string>

#include "parse.h"
#include "lex.h"
#include "eval.h"

namespace lambda
{
    auto parse_numeral(const std::string& str) -> ParseResult;

    auto contract_numeral(const Term& term) -> Term;

    auto from_numeral(const Term& term) -> std::optional<int>;

    auto to_numeral(uint val) -> Term;
}

#endif //LAMBDA_NUMERALS_H
