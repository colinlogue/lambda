//
// Created by colin on 6/6/20.
//

#include "prelude.h"

#include <filesystem>
#include <fstream>
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
        std::filesystem::path prelude_path {"../data/prelude.lam"};
        std::fstream file {prelude_path};
        auto context {*parse_file(file).get_ok()};
        return context;
    }
}