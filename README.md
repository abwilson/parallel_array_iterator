# parallel_array_iterator
Allow parallel arrays to be used in std algorithms where you would use arrays of structures.

# Array of Structures or Structure of Arrays?

Lets say we want to keep a bunch of 64 bit ints indexed by 16 bit ints. We could of course use a std::map, or std::unordered_map,
but both of these have draw backs in terms of memory locality and efficiency. Lets also say we are memory constrained and we
want a packed contiguous structure where we have many more look-ups than inserts. We might think about using a sorted array
and binary search. The obvious way to do this would be define a struct to store the key and value and have an array of these
which is kept sorted using std::sort and searched using std::equal_range. This is known as an array of structures.

There are a couple of draw backs to this approach. Firstly if we store a uint16_t and a uin64_t together then the struct will
be aligned to 64 bit boundaries. We'll thus be wasting 6 bytes for each entry. Secondly when we come to search the array,
although we are only comparing keys we still have to bring all the values into cache too. This will lead to considerable more
cache misses than if we were searching through the keys alone.

We can fix both of these problems by reversing the organisation. Instead of keeping an array of structures, we keep a structure
of arrays. We have separate arrays for keys and values. The keys[0] corresponds to values[0] etc. This means the keys are all
packed together so we're not wasting any space and when we come to search we no longer have to load the values. The trouble is
how do we sort this? std::sort isn't going to work because because we have to organise the values to match the ordering of the
keys. Also although we can use the standard algorthims to search in the keys we then have to do some pointer arithmetic to map
the found key onto the corresponding value. So, while structure of arrays is more efficient, it's also more of a pain to
implement.

Is there a way we can use the structure of arrays technique but make it compatible with standard algorithms? Thats what this
library is about. With parallet_array_iterator we can do the following:


```c++
auto example()
{
    constexpr auto capicity = 1024;
    // Some data.
    std::uint16_t keys[capacity];
    double values[capacity];
    std::array<char, capacity> flags;
    std::vectory<std::string> names;
    
    // Bring in special definitions of begin and end.
    using namespace parallel_array_iterator;
    
    // Define a comparitor which looks at the first slot in a tuple.
    auto cmp1st = [](auto&& lhs, auto&& rhs)
        { return std::get<0>(lhs) < std::get<0>(rhs); };
    
    // Sort our data by keys.
    std::sort(
        begin(keys, values, flags, names),
        end(keys, values, flags, names),
        cmp1st);
    
    // Pull out the name with this key.
    return *std::get<3>(
        std::lower_bound(
            begin(keys, values, flags, names),
            end(keys, values, flags, names),
            std::make_tuple(42),
            cmp1st));
}
```
