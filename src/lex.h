//
// Created by colin on 6/2/20.
//

#ifndef LAMBDA_LEX_H
#define LAMBDA_LEX_H

#include <iostream>
#include <vector>

#include "lang_tools/lexer/lexer.hpp"

namespace lambda
{
    enum class TokenType {Lambda, Name, Dot, LeftParen, RightParen};
    struct Token
    {
        TokenType type;
        std::string value {};
    };

    using LexResult = lang_tools::LexResult<Token>;

    auto read(std::istream& in)  -> std::vector<Token>;
    auto read(std::string in)    -> std::vector<Token>;

    auto as_string(TokenType token) -> std::string;
    auto lex(std::istream& in) -> lang_tools::LexResult<Token>;

    class TokenStream
    {
    public:
        explicit TokenStream(std::istream& source);

        auto begin() -> lang_tools::token_iterator<Token>;
        auto end()   -> lang_tools::token_iterator<Token>;

    private:
        std::istream& source;
    };
}

#endif //LAMBDA_LEX_H
