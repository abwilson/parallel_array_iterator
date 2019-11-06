#include <utility>
#include <tuple>
#include <algorithm>

template<typename... Ts>
struct ParallelArrayReference: std::tuple<Ts&...>
{
    using std::tuple<Ts&...>::tuple;
    using std::tuple<Ts&...>::operator=;

    operator std::tuple<Ts...>() const { return *this; }
};

template<typename... Ts>
struct ParallelArrayIterator: std::tuple<Ts*...>
{
    using std::tuple<Ts*...>::tuple;
    
    using value_type        = std::tuple<Ts...>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void;
    using reference         = ParallelArrayReference<Ts...>;
    using iterator_category = std::random_access_iterator_tag;

    using Index = std::index_sequence_for<Ts...>;

    static constexpr auto makeIndex(){ return Index{}; }

    template<typename Op, std::size_t... idx>
    static constexpr auto forEachIndexImpl(
        Op&& op, std::index_sequence<idx...> i)
    {
        return op(std::integral_constant<std::size_t, idx>{}...);
    }

    template<typename Op>
    static constexpr auto forEachIndex(Op&& op)
    {
        return forEachIndexImpl(std::forward<Op>(op), Index{});
    }

    template<typename... X>
    static constexpr void expandPack(X&&...){}

    auto makeReference() const
    {
        return forEachIndex(
            [&](auto... idx){ return reference{*std::get<idx>(*this)...}; });
    }

    auto operator*() const { return makeReference(); }

    friend auto operator+(const ParallelArrayIterator& lhs, int rhs)
    {
        return forEachIndex(
            [&lhs, rhs](auto... idx)
            {
                return ParallelArrayIterator{(std::get<idx>(lhs) + rhs)...};
            });
    }

    auto operator+=(int rhs)
    {
        return forEachIndex(
            [&](auto... idx)
            {
                expandPack(std::get<idx>(*this) += rhs...);
                return *this;
            });
    }

    auto operator++(){ return operator+=(1); }

    friend auto operator-(const ParallelArrayIterator& lhs,
        const ParallelArrayIterator& rhs)
    {
        return std::get<0>(lhs) - std::get<0>(rhs);
    }

    friend auto operator-(const ParallelArrayIterator& lhs, int rhs)
    {
        return lhs + -rhs;
    }

    auto operator--()
    {
        return operator+=(-1);
    }
};

#if 1
namespace std
{
template<typename... Ts>
void swap(
    ParallelArrayReference<Ts...>&& lhs, 
    ParallelArrayReference<Ts...>&& rhs)
{
    std::tuple<Ts...> tmp{static_cast<std::tuple<Ts&...> >(lhs)};
    lhs = rhs;
    rhs = tmp;
}
}
#endif

template<std::size_t size, typename... Ts>
auto begin(Ts (&...arrays)[size])
{
    return ParallelArrayIterator<Ts...>{arrays...};
}

template<std::size_t size, typename... Ts>
auto end(Ts (&...arrays)[size])
{
    return ParallelArrayIterator<Ts...>{std::end(arrays)...};
}

constexpr auto size = 1024;

char keys[size];
uint64_t values[size];

void test()
{
    auto i = begin(keys, values);
    auto d = i - i;
    auto j = i - 1;

    std::swap(i, j);

    i.makeIndex();

    i += 1;
}

void bar()
{
    using Tup = std::tuple<int, char, bool>;
    Tup t1;
    Tup t2;
    std::swap(t1, t2);
}
#if 1
void foo()
{
    std::sort(
        begin(keys, values),
        end(keys,values),
        [](const auto &lhs, const auto& rhs)
        { return std::get<0>(lhs) < std::get<0>(rhs); });
}
#endif
