//
// Created by colin on 6/2/20.
//

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>

#include "lang_tools/utils/utils.h"

#include "lex.h"

using std::optional;
using std::string;
using std::stringstream;
using lang_tools::LexResult;
using lang_tools::is_whitespace;
using lang_tools::lowercase;

namespace lambda
{

    auto to_token(string str) -> optional<Token>
    {
        string name {lowercase(str)};
        optional<Token> maybe;

        // get next token
        if (name == "(")
            maybe = {TokenType::LeftParen, name};

        else if (name == ")")
            maybe = {TokenType::RightParen, name};

        else if (name == "\\")
            maybe = {TokenType::Lambda, name};

        else if (name == ".")
            maybe = {TokenType::Dot, name};

        else if (std::all_of(name.begin(), name.end(),
                [] (const char c) -> bool { return std::isalpha(c); }))
            maybe = {TokenType::Name, name};

        return maybe;
    }

    auto lex(std::istream& in) -> LexResult<Token>
    {
        char c;
        stringstream buffer {};

        // skip to next non-whitespace character
        while (in.good() && is_whitespace(in.peek()))
        {
            in.get();
        }

        // read in until next whitespace
        while (in.get(c) && !is_whitespace(c))
        {
            buffer << c;
        }

        // try to match token
        std::optional<Token> maybe_token {to_token(buffer.str())};
        if (maybe_token.has_value())
            return LexResult<Token>::make_ok(maybe_token.value());

        // otherwise return failure
        return LexResult<Token>::make_err("Unable to match token: " + buffer.str());
    }

    auto as_string(TokenType token) -> std::string
    {
        switch (token)
        {
            case TokenType::LeftParen:
                return "LEFT_PAREN";

            case TokenType::RightParen:
                return "RIGHT_PAREN";

            case TokenType::Lambda:
                return "LAMBDA";

            case TokenType::Dot:
                return "DOT";

            case TokenType::Name:
                return "NAME";
        }

        throw std::logic_error("Unknown token type");
    }


    TokenStream::TokenStream(std::istream& source)
        : source {source}
    {}

    auto TokenStream::begin() -> lang_tools::token_iterator<Token>
    {
        return lang_tools::token_iterator<Token>(lex, source);
    }

    auto TokenStream::end() -> lang_tools::token_iterator<Token>
    {
        return lang_tools::token_iterator<Token>();
    }

}
