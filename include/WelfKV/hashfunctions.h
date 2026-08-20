#pragma once
#include <WelfKV/common.h>
#include <cstdint>
#include <xxhash.h>

template<typename Key>
struct xxhash_function
{
    static uint64_t hash(Key key)
    {
        return XXH3_64bits(key.data(), key.size_bytes());
    }
};
