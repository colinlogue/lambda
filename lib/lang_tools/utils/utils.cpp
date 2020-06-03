//
// Created by colin on 5/20/20.
//

#include <string>

using std::string;

namespace lang_tools
{
    auto is_whitespace(char c) -> bool
    {
        return c == ' ';
    }

    auto lowercase(string str) -> string
    {
        std::transform(str.begin(), str.end(), str.begin(), tolower);
        return str;
    }
}