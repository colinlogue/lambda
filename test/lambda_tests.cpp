//
// Created by colin on 6/3/20.
//
#include <iostream>
#include <numerals.h>

#include "shared_tests.h"
#include "helpers.h"

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
        it("can parse application", [&]() {
            std::string term {"(\\t.\\f.t) x y"};
            std::optional<Term> parse_result {parse_string(term)};
            AssertThat(parse_result.has_value(), IsTrue());
            if (parse_result.has_value())
            {
                Term val {parse_result.value()};
                Term expected {app(app(tru, "x"), "y")};
                AssertThat(val, Equals(expected));
            }
        });
        it("can parse parenthesized variable", [&]() {
            std::string term{"(x)"};
            std::optional<Term> parse_result {parse_string(term)};
            AssertThat(parse_result.has_value(), IsTrue());
            Term val {parse_result.value()};
            AssertThat(val, Equals(x));
        });
        it("can parse parenthesized application", [&]() {
            parse_test("(x y)", app("x", "y"));
        });
        it("can parse parenthesized abstraction", [&]() {
            parse_test("\\x.x", lam("x", x));
        });
    });
    describe("numeral tests", []() {
        it("can contract zero", []() {
            Term term {contract_numeral(parse_string("0").value())};
            AssertThat(term, Equals(var("0")));
        });
        it("can contract one", []() {
            Term term {contract_numeral(parse_string("1").value())};
            AssertThat(term, Equals(var("1")));
        });
        it("can contract two", []() {
            Term term {contract_numeral(parse_string("2").value())};
            AssertThat(term, Equals(var("2")));
        });
    });
});
