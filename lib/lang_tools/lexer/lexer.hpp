//
// Created by colin on 5/20/20.
//

#ifndef LANG_TOOLS_LEX_H
#define LANG_TOOLS_LEX_H

#include <functional>
#include <optional>
#include <string>
#include <stdexcept>

#include "result/Result.hpp"


namespace lang_tools
{

    using LexErr = std::string;

    template <typename Token>
    using LexResult = result::Result<Token, LexErr>;

    template <typename Token>
    using Lexer = std::function<LexResult<Token>(std::istream&)>;

    template<typename Token>
    class token_iterator
    {
    public:
        // TODO: what should difference_type be?
        using difference_type = std::ptrdiff_t;
        using value_type = LexResult<Token>;
        using reference = LexResult<Token>&;
        using pointer = LexResult<Token>*;
        using iterator_category = std::input_iterator_tag;

        token_iterator() = default;
        token_iterator(Lexer<Token> reader, std::istream& source);
        token_iterator(Lexer<Token> reader, std::istream&& source) = delete;

        template <typename T>
        friend auto operator ==(const token_iterator<T>& lhs, const token_iterator<T>& rhs) -> bool;

        template <typename T>
        friend auto operator !=(const token_iterator<T>& lhs, const token_iterator<T>& rhs) -> bool;

        auto operator ++()    -> token_iterator&;
        auto operator ++(int) -> token_iterator;
        auto operator  *()    -> LexResult<Token>&;
        auto operator ->()    -> LexResult<Token>*;

        auto read_next() -> LexResult<Token>&;

    private:

        struct data_t
        {
        public:
            const Lexer<Token> reader;
            std::istream& source;
            LexResult<Token> current;
            data_t(Lexer<Token> reader, std::istream& source)
                : reader {reader}, source {source}, current {reader(source)}
            {}
        };

        std::optional<data_t> data {};

        auto unsafe_get_current() -> LexResult<Token>&;
        auto unsafe_get_reader()  -> const Lexer<Token>&;
        auto unsafe_get_source()  -> std::istream&;
    };


    ////////////////////////////////////////////////////////////////////////
    /////   Operators
    ////////////////////////////////////////////////////////////////////////

    template<typename T>
    auto operator!=(const token_iterator<T>& lhs, const token_iterator<T>& rhs) -> bool
    {
        // ending condition is that one is null
        // TODO: check this against how istream_iterator != is implemented
        return !(lhs == rhs);
    }

    template<typename Token>
    auto token_iterator<Token>::operator++() -> token_iterator&
    {
        read_next();
        return *this;
    }

    template<typename Token>
    auto token_iterator<Token>::operator++(int) -> token_iterator
    {
        token_iterator copy = *this;
        read_next();
        return std::move(copy);
    }

    template<typename Token>
    auto token_iterator<Token>::read_next() -> LexResult<Token>&
    {
        LexResult<Token>& current = unsafe_get_current();
        const Lexer<Token>& reader = unsafe_get_reader();
        std::istream& source = unsafe_get_source();
        if (source.peek() < 0)
            data.reset();
        else
            current = reader(source);
        return current;
    }

    /**
     * @tparam Token    The type of token the lexer produces.
     * @return          A reference to the last extracted token.
     * @throws          std::out_of_range if no token is available.
     */
    template<typename Token>
    auto token_iterator<Token>::operator*() -> LexResult<Token>&
    {
        if (!(data.has_value()))
            throw std::out_of_range("no result available");
        return unsafe_get_current();
    }

    template<typename Token>
    auto token_iterator<Token>::operator->() -> LexResult<Token>*
    {
        return &(unsafe_get_current());
    }

    template<typename T>
    auto operator ==(const token_iterator<T>& lhs, const token_iterator<T>& rhs) -> bool
    {
        return bool(lhs.data) == bool(rhs.data);
    }

    template<typename Token>
    token_iterator<Token>::token_iterator(Lexer<Token> reader, std::istream& source)
        : data { std::in_place, reader, source }
    {}

    template<typename Token>
    auto token_iterator<Token>::unsafe_get_current() -> LexResult<Token>&
    {
        return data.value().current;
    }

    template<typename Token>
    auto token_iterator<Token>::unsafe_get_reader() -> const Lexer<Token>&
    {
        return data.value().reader;
    }

    template<typename Token>
    auto token_iterator<Token>::unsafe_get_source() -> std::istream&
    {
        return data.value().source;
    }

}

#endif //LANG_TOOLS_LEX_H
