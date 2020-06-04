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
using lang_tools::is_whitespace;
using lang_tools::lowercase;

namespace lambda
{
    auto lex(std::istream& in) -> LexResult
    {
        char c;
        stringstream buffer {};

        // skip to next non-whitespace character
        while (in.good() && is_whitespace(in.peek()))
        {
            in.get();
        }

        // check for single-character tokens
        optional<TokenType> ttype;
        switch (in.peek())
        {
            case '\\':
                ttype = TokenType::Lambda;
                break;
            case '.':
                ttype = TokenType::Dot;
                break;
            case '(':
                ttype = TokenType::LeftParen;
                break;
            case ')':
                ttype = TokenType::RightParen;
                break;
        }

        // if matched any tokens, return
        if (ttype.has_value())
        {
            in.get(c);
            return LexResult::make_ok(Token{ttype.value(), string{c}});
        }

        // otherwise, try to read as name
        if (std::isalpha(in.peek()))
        {
            // read in until next non-alpha char
            while (std::isalpha(in.peek()))
            {
                in.get(c);
                buffer << c;
            }

            return LexResult::make_ok(Token{TokenType::Name, buffer.str()});
        }

        // if no matches, return failure
        in.get(c);
        return LexResult::make_err("Unable to match character: " + string {c});
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

    /**
     * This currently just returns a vector of all the tokens that were able to
     * be parsed and just discards any that failed. That's obviously not great
     * behavior so planning to revamp this at some point.
     */
    auto read(std::istream& in)  -> std::vector<Token>
    {
        TokenStream stream {in};
        std::vector<Token> tokens {};
        for (auto token : stream)
        {
            if (token.is_ok())
                tokens.push_back(*token.get_ok());
        }
        return tokens;
    }

    auto read(std::string in)    -> std::vector<Token>
    {
        std::stringstream stream {in};
        return read(stream);
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
