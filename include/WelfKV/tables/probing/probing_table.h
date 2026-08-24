#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>

// probing_table is the base for tables that use probing as their collision resolution mechanism.
template<typename Key, typename Value, typename BucketType, typename HashFn>
class probing_table
{
    public:
        using key_type = Key;
        using value_type = Value;
        using bucket_type = BucketType;
        using hash_function_type = HashFn;

    protected:
        std::vector<BucketType> buckets_;
        uint64_t seed_;

        probing_table(std::size_t num_buckets, uint64_t seed)
            : buckets_(num_buckets)
            , seed_(seed)
        {
        }

        std::size_t start_slot(Key key) const
        {
            return static_cast<std::size_t>(HashFn::hash(key, seed_) % buckets_.size());
        }
};
