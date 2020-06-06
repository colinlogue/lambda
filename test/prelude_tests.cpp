//
// Created by colin on 6/5/20.
//

#include "shared_tests.h"

#include "prelude.h"
#include "helpers.h"

static const Context prelude {get_prelude()};

auto run_test(std::string term_str, const Term& expected) -> void
{
    std::optional<Term> parse_result {parse_string(term_str)};
    Term actual {reduce(parse_result.value(), prelude)};
    AssertThat(actual, Equals(expected));
}

go_bandit([]() {
    describe("prelude tests", []() {
        it("and true true == true", []() {
            run_test("and true true", prelude.find("true")->second);
        });
        it("and with false is false", []() {
            Term fls {prelude.find("false")->second};
            run_test("and false false", fls);
            run_test("and true false", fls);
            run_test("and false true", fls);
        });
    });
});