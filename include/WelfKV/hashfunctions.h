#pragma once
#include <WelfKV/common.h>
#include <cstdint>
#include <xxhash.h>

template<typename Key>
struct xxhash_function
{
    static uint64_t hash(Key key, uint64_t seed = 0)
    {
        return XXH3_64bits_withSeed(key.data(), key.size_bytes(), seed);
    }
};
