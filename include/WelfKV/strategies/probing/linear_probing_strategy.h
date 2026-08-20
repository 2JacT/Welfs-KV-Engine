#pragma once
#include <WelfKV/strategies/probing/probing_strategy.h>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>

/*
Linear_Probing_Strategy is the strategy that uses linear probing as its collision resolution mechanism.
*/
template<typename Key, typename Value, typename BucketType, typename HashFn>
struct linear_probing_strategy : Probing_Strategy<Key, Value, BucketType, HashFn>
{
    using key_type = Key;
    using value_type = Value;
    using bucket_type = BucketType;
    using hash_function_type = HashFn;

    static constexpr bool uses_probing = true;

    static bool insert(Key key, Value value, std::vector<BucketType>& buckets)
    {
        const std::size_t bucket_count = buckets.size();
        if (bucket_count == 0)
        {
            return false;
        }

        std::size_t index = Probing_Strategy<Key, Value, BucketType, HashFn>::start_slot(buckets, key);
        for (std::size_t probe = 0; probe < bucket_count; ++probe)
        {
            const std::size_t slot = (index + probe) % bucket_count;
            auto& bucket = buckets[slot];

            if (bucket.is_empty() || bucket.contains(key))
            {
                return bucket.insert(key, value);
            }
        }
        return false;
    }

    static bool lookup(const Key& key, Value& value, std::vector<BucketType>& buckets)
    {
        const std::size_t bucket_count = buckets.size();
        if (bucket_count == 0)
        {
            return false;
        }

        std::size_t index = Probing_Strategy<Key, Value, BucketType, HashFn>::start_slot(buckets, key);
        for (std::size_t probe = 0; probe < bucket_count; ++probe)
        {
            const std::size_t slot = (index + probe) % bucket_count;
            auto& bucket = buckets[slot];

            if (bucket.is_empty())
            {
                return false;
            }
            if (bucket.contains(key))
            {
                return bucket.get(key, value);
            }
        }
        return false;
    }

    static bool update(Key key, Value value, std::vector<BucketType>& buckets)
    {
        return insert(key, value, buckets);
    }

    static bool remove(const Key& key, std::vector<BucketType>& buckets)
    {
        const std::size_t bucket_count = buckets.size();
        if (bucket_count == 0)
        {
            return false;
        }

        std::size_t index = Probing_Strategy<Key, Value, BucketType, HashFn>::start_slot(buckets, key);
        for (std::size_t probe = 0; probe < bucket_count; ++probe)
        {
            const std::size_t slot = (index + probe) % bucket_count;
            auto& bucket = buckets[slot];

            if (bucket.is_empty())
            {
                return false;
            }
            if (bucket.contains(key))
            {
                return bucket.remove(key);
            }
        }
        return false;
    }
};
