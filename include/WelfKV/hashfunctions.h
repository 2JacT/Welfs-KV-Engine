#pragma once
#include <WelfKV/common.h>
#include <xxhash.h>

class HashFunction
{
    public:
        virtual uint64_t Hash(Key key) = 0;
};

class XXHash : public HashFunction
{
    public:
        uint64_t Hash(Key key)
        {
            XXH64_hash_t hash = XXH3_64bits(key.data(), key.size_bytes());
            return hash;
        };
};
