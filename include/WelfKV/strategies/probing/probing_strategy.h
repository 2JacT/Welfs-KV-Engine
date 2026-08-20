#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>

/*
Probing_Strategy is the base class for all strategies that use probing as their collision resolution mechanism.
*/
template<typename Key, typename Value, typename BucketType, typename HashFn>
struct Probing_Strategy
{
    using key_type = Key;
    using value_type = Value;
    using bucket_type = BucketType;
    using hash_function_type = HashFn;

    protected:
        static std::size_t start_slot(const std::vector<BucketType>& buckets, Key key)
        {
            return static_cast<std::size_t>(HashFn::hash(key) % buckets.size());
        }
};