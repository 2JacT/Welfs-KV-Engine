#pragma once
#include <WelfKV/common.h>
#include <xxhash.h>

template<typename Config>
class HashFunction
{
    public:
        virtual uint64_t Hash(typename Config::Key key) = 0;
        virtual ~HashFunction() = default;
};

template<typename Config>
class XXHash : public HashFunction<Config>
{
    public:
        uint64_t Hash(typename Config::Key key) override
        {
            XXH64_hash_t hash = XXH3_64bits(key.data(), key.size_bytes());
            return hash;
        }
};
