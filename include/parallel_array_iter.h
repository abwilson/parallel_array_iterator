#pragma once
#include <utility>
#include <tuple>
#include <algorithm>
#include <string>
#include <array>

template<typename Op, std::size_t... idx>
static constexpr auto applyIndex(
    Op&& op, std::index_sequence<idx...> i)
{
    return op(std::integral_constant<std::size_t, idx>{}...);
}

template<typename... Ts>
struct ParallelArrayReference: std::tuple<Ts&...>
{
    using std::tuple<Ts&...>::tuple;
    using std::tuple<Ts&...>::operator=;

    operator std::tuple<Ts...>() const { return *this; }
};

namespace std
{
template<typename... Ts>
void swap(
    ParallelArrayReference<Ts...>&& lhs, 
    ParallelArrayReference<Ts...>&& rhs)
{
    std::swap(lhs, rhs);
}
}

template<typename... Is>
struct ParallelArrayIterator: std::tuple<Is...>
{
    using std::tuple<Is...>::tuple;
    
    using value_type        = std::tuple<
        typename std::iterator_traits<Is>::value_type...>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void;
    using reference         = ParallelArrayReference<
        typename std::iterator_traits<Is>::value_type...>;
    using iterator_category = std::random_access_iterator_tag;

    using Index = std::index_sequence_for<Is...>;

    template<typename Op>
    static constexpr auto forEachIter(Op&& op)
    {
        return applyIndex(std::forward<Op>(op), Index{});
    }

    template<typename... X>
    static constexpr void expandPack(X&&...){}

    auto makeReference() const
    {
        return forEachIter(
            [&](auto... idx){ return reference{*std::get<idx>(*this)...}; });
    }

    auto operator*() const { return makeReference(); }

    friend auto operator+(const ParallelArrayIterator& lhs, int rhs)
    {
        return forEachIter(
            [&lhs, rhs](auto... idx)
            {
                return ParallelArrayIterator{(std::get<idx>(lhs) + rhs)...};
            });
    }

    auto operator+=(int rhs)
    {
        return forEachIter(
            [&](auto... idx)
            {
                expandPack(std::get<idx>(*this) += rhs...);
                return *this;
            });
    }

    auto operator++(){ return operator+=(1); }
    auto operator--(){ return operator+=(-1); }

    friend auto operator-(const ParallelArrayIterator& lhs,
        const ParallelArrayIterator& rhs)
    {
        return std::get<0>(lhs) - std::get<0>(rhs);
    }

    friend auto operator-(const ParallelArrayIterator& lhs, int rhs)
    {
        return lhs + -rhs;
    }
};
#if 1
template<typename... Ts>
auto begin(Ts&... arrays)
    -> ParallelArrayIterator<decltype(std::begin(arrays))...>
{
    return { std::begin(arrays)... };
}

template<typename... Ts>
auto end(Ts&... arrays)
    -> ParallelArrayIterator<decltype(std::end(arrays))...>
{
    return { std::end(arrays)... };
}

template<std::size_t size, typename... Ts>
auto begin(Ts (&...arrays)[size])
    -> ParallelArrayIterator<decltype(std::begin(arrays))...>
{
    return { std::begin(arrays)... };
}

template<std::size_t size, typename... Ts>
auto end(Ts (&...arrays)[size])
    -> ParallelArrayIterator<decltype(std::end(arrays))...>
{
    return { std::end(arrays)... };
}

#else
template< class C >
auto begin( C& c ) -> decltype(c.begin())
{
    return c.begin();
}

template< class C >
auto end( C& c ) -> decltype(c.end())
{
    return c.end();
}

#endif

