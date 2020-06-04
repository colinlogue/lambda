//
// Created by colin on 6/3/20.
//
#include <iostream>

#include <bandit/bandit.h>

#include "eval.h"
#include "helpers.h"

using namespace snowhouse;
using namespace bandit;

using namespace lambda;


go_bandit([]() {
    const Term ident {lam("x",var("x"))};
    const Term tru   {lam("t", lam("f", var("t")))};
    const Term fls   {lam("t", lam("f", var("f")))};
    const Term x     {var("x")};
    const Term y     {var("y")};
    describe("comparison tests", [&]() {
        it("variables with different names compare false", [&]() {
            AssertThat(compare(x,y), IsFalse());
        });
        it("variable compares false to abstraction", [&]() {
            AssertThat(compare(x,tru), IsFalse());
            AssertThat(compare(var("t"), tru), IsFalse());
            AssertThat(compare(var("f"), tru), IsFalse());
        });
        it("different abstractions compare false", [&]() {
            AssertThat(compare(tru,fls), IsFalse());
            AssertThat(compare(fls,tru), IsFalse());
        });
    });
    describe("parse tests", [&]() {
        it("can parse single variable", [&]() {
            AssertThat(parse_string("x").value(), Equals(x));
            AssertThat(parse_string("y").value(), Equals(y));
            AssertThat(parse_string("z").value(), Is().Not().EqualTo(x));
            AssertThat(parse_string("longvarname").value(), Equals(var("longvarname")));
        });
        it("can parse identity abstraction", [&]() {
            AssertThat(parse_string("\\x.x"), Equals(ident));
        });
        it("can parse tru", [&]() {
            std::string tru_str {"\\t.\\f.t"};
            std::optional<Term> parse_result {parse_string(tru_str)};
            AssertThat(parse_result.has_value(), IsTrue());
            if (parse_result.has_value())
            {
                AssertThat(compare(parse_result.value(), tru), IsTrue());
            }
        });
    });
    describe("reduction tests", [&]() {
        it ("tru doesn't reduce", [&]() {
            AssertThat(reduce(tru), Equals(tru));
        });
    });
});

int main(int argc, char* argv[]) {
    return bandit::run(argc, argv);
}