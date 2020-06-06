//
// Created by colin on 6/6/20.
//

#include "prelude.h"

#include <string>
#include <utility>

namespace lambda
{
    auto unsafe_parse(std::string term_str) -> Term
    {
        return parse_string(std::move(term_str)).value();
    }

    auto get_prelude() -> lang_tools::Context<Term>
    {
        return {
            // booleans
            {"true",    unsafe_parse("\\t.\\f.t")},
            {"false",   unsafe_parse("\\t.\\f.f")},
            {"and",     unsafe_parse("\\b.\\c. b c false")},
//            {"or",      unsafe_parse("")},
            // pairs
            {"pair",    unsafe_parse("\\f.\\s.\\b. b f s")},
            {"first",   unsafe_parse("\\p. p true")},
            {"second",  unsafe_parse("\\p. p false")},
            // numerals
            {"zero",    unsafe_parse("\\s.\\z.z")},
            {"succ",    unsafe_parse("\\n.\\s.\\z.s (n s z)")},
            {"plus",    unsafe_parse("\\m.\\n.\\s.\\z.m s (n s z)")},
            {"times",   unsafe_parse("\\m.\\n. m (plus n) zero")}
        };
    }
}