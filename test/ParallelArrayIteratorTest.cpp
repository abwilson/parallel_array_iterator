#include <parallel_array_iter.h>

constexpr auto size = 1024;

char keys[size] = {0};
uint64_t values[size] = {0};
std::string names[size];
std::array<double, size> prices = {0};

auto cmp1st = [](const auto &lhs, const auto& rhs)
{ return std::get<0>(lhs) < std::get<0>(rhs); };

auto test()
{
    auto i = begin(keys, values, names, prices);
    auto e = end(keys, values, names, prices);    
    auto d = i - i;
    auto j = i - 1;

    std::swap(i, j);

    i += 1;

    return std::get<3>(*i);
}

#if 1
auto foo()
{
    std::sort(
        begin(keys, values, names, prices),
        end(keys,values, names, prices),
        cmp1st);
}
#if 0
auto foo2()
{
    auto found = std::lower_bound(
        begin(keys, values, names, prices),
        end(keys,values, names, prices),
        std::make_tuple('A'),
        cmp1st);
    return found == end(keys, values, names, prices) ? 3.14 : 
        *std::get<3>(found);
}
#else
auto foo2()
{
    return std::get<1>(
        std::lower_bound(
            begin(keys, values, names, prices),
            end(keys,values, names, prices),
            std::make_tuple('A'),
            cmp1st));
}
#endif
#endif

