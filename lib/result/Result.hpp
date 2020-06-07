#ifndef RESULT_HPP
#define RESULT_HPP

#include <functional>
#include <initializer_list>
#include <optional>
#include <variant>


namespace result
{
    template <typename OkType, typename ErrType>
    class Result
    {
    public:
        using data_type = std::variant<OkType, ErrType>;

        // constructors
        explicit Result(const data_type& data);
        Result(const Result& other);
        Result(Result&& result) noexcept;

        // operators
        auto operator =(Result&& other) noexcept -> Result& = default;
        auto operator =(OkType ok) -> Result&;
        auto operator =(ErrType err) -> Result&;

        template <typename T, typename U>
        friend auto operator ==(const Result<T,U>& lhs, const Result<T,U>& rhs) -> bool;

        // factories
        template <typename ...Args>
        static auto make_ok(Args ...args) -> Result;

        template <typename ...Args>
        static auto make_err(Args ...args) -> Result;

        // accessors
        auto get_ok() -> OkType*;

        auto get_err() -> ErrType*;

        auto is_ok() const -> bool;

        auto is_err() const -> bool;

        // operations
        template <typename OkB>
        auto map_ok(std::function<OkB(OkType)> f) -> Result<OkB, ErrType>;

        template <typename ErrB>
        auto map_err(std::function<ErrB(ErrType)> f) -> Result<OkType, ErrB>;

        template <typename OkB, typename ErrB>
        auto map_both(std::function<OkB(OkType)> f, std::function<ErrB(ErrType)> g) -> Result<OkB, ErrB>;

        template <typename OkB>
        auto and_then(std::function<Result<OkB, ErrType>(OkType)> f) -> Result<OkB, ErrType>;

    private:
        constexpr static size_t ok_index = 0;
        constexpr static size_t err_index = 1;
        constexpr static std::in_place_index_t<0> ok_place {};
        constexpr static std::in_place_index_t<1> err_place {};
        data_type data;
    };

    template<typename OkType, typename ErrType>
    template<typename... Args>
    auto Result<OkType, ErrType>::make_ok(Args... args) -> Result
    {
        // have to explicitly specify index in case OkType and ErrType are the same
        auto data = std::variant<OkType, ErrType>(ok_place, args...);
        return Result(data);
    }

    template<typename OkType, typename ErrType>
    template<typename... Args>
    auto Result<OkType, ErrType>::make_err(Args... args) -> Result
    {
        // have to explicitly specify index in case OkType and ErrType are the same
        auto data = std::variant<OkType, ErrType>(err_place, args...);
        return Result(data);
    }

    template<typename OkType, typename ErrType>
    Result<OkType, ErrType>::Result(const Result& other)
            : data {other.data}
    {}

    template<typename OkType, typename ErrType>
    Result<OkType, ErrType>::Result(const Result::data_type& data)
            : data {data}
    {}

    // move constructor
    template<typename OkType, typename ErrType>
    Result<OkType, ErrType>::Result(Result&& result) noexcept
            : data {std::move(result.data)}
    {}

    template<typename OkType, typename ErrType>
    template<typename OkB>
    auto Result<OkType, ErrType>::and_then(std::function<Result<OkB,ErrType>(OkType)> f) -> Result<OkB, ErrType>
    {
        auto ok = std::get_if<ok_index>(&data);
        if (ok)
            return f(*ok);

        return Result<OkB, ErrType>::make_err(*get_err());
    }

    template<typename OkType, typename ErrType>
    template<typename OkB>
    auto Result<OkType, ErrType>::map_ok(std::function<OkB(OkType)> f) -> Result<OkB, ErrType>
    {
        auto ok = std::get_if<ok_index>(&data);
        if (ok)
            return Result<OkB, ErrType>::make_ok(f(*ok));

        return Result<OkB, ErrType>::make_err(*get_err());
    }

    template<typename OkType, typename ErrType>
    template<typename ErrB>
    auto Result<OkType, ErrType>::map_err(std::function<ErrB(ErrType)> f) -> Result<OkType, ErrB>
    {
        auto err = std::get_if<err_index>(&data);
        if (err)
            return Result<OkType, ErrB>::make_err(f(*err));

        return Result<OkType, ErrB>::make_ok(*get_ok());
    }

    template<typename T, typename U>
    auto operator==(const Result<T,U>& lhs, const Result<T,U>& rhs) -> bool
    {
        return lhs.data == rhs.data;
    }

    template<typename OkType, typename ErrType>
    auto Result<OkType, ErrType>::get_ok() -> OkType*
    {
        return std::get_if<ok_index>(&data);
    }

    template<typename OkType, typename ErrType>
    auto Result<OkType, ErrType>::get_err() -> ErrType*
    {
        return std::get_if<err_index>(&data);
    }

    template<typename OkType, typename ErrType>
    auto Result<OkType, ErrType>::is_ok() const -> bool
    {
        return std::get_if<ok_index>(&data) != nullptr;
    }

    template<typename OkType, typename ErrType>
    auto Result<OkType, ErrType>::is_err() const -> bool
    {
        return std::get_if<err_index>(&data) != nullptr;
    }

    template<typename OkType, typename ErrType>
    auto Result<OkType, ErrType>::operator =(OkType ok) -> Result&
    {
        data = ok;
        return *this;
    }

    template<typename OkType, typename ErrType>
    template<typename OkB, typename ErrB>
    auto
    Result<OkType, ErrType>::map_both(std::function<OkB(OkType)> f, std::function<ErrB(ErrType)> g) -> Result<OkB, ErrB>
    {
        if (is_ok())
            return Result<OkB, ErrB>::make_ok(f(*get_ok()));
        return Result<OkB, ErrB>::make_err(g(*get_err()));
    }

    template<typename OkType, typename ErrType>
    auto Result<OkType, ErrType>::operator =(ErrType err) -> Result&
    {
        data = err;
        return *this;
    }
}

#endif //ARITH_RESULT_HPP
