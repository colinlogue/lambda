//
// Created by colin on 6/2/20.
//

#ifndef LANG_TOOLS_PARSE_HPP
#define LANG_TOOLS_PARSE_HPP

#include <functional>
#include <string>
#include <queue>

#include "result/Result.hpp"

namespace lang_tools
{
    using ParseErr = std::string;

    template <typename Term>
    using ParseResult = result::Result<Term, ParseErr>;

    template <typename Term, typename Token>
    using Parser = std::function<ParseResult<Term>(std::queue<Token>)>;

}

#endif //LANG_TOOLS_PARSE_HPP
