//
// Created by colin on 6/5/20.
//

#include "shared_tests.h"

auto parse_test(std::string term_str, const lambda::Term& expected) -> void
{
    std::optional<lambda::Term> parse_result {lambda::parse_string(term_str)};
    AssertThat(parse_result.has_value(), IsTrue());
    AssertThat(parse_result.value(), Equals(expected));
}
