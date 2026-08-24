#pragma once
#include <WelfKV/tables/probing/probing_table.h>
#include <cstddef>
#include <cstdint>
#include <vector>

template<typename Key, typename Value, typename BucketType, typename HashFn>
class linear_probing_table : public probing_table<Key, Value, BucketType, HashFn>
{
    public:
        using key_type = Key;
        using value_type = Value;
        using bucket_type = BucketType;
        using hash_function_type = HashFn;

        static constexpr bool uses_probing = true;
        static constexpr bool requires_addressed_bucket = false;

        explicit linear_probing_table(std::size_t num_buckets, uint64_t seed = 0)
            : probing_table<Key, Value, BucketType, HashFn>(num_buckets, seed)
        {
        }

        bool insert(Key key, Value value)
        {
            const std::size_t bucket_count = this->buckets_.size();
            if (bucket_count == 0)
            {
                return false;
            }

            std::size_t index = this->start_slot(key);
            for (std::size_t probe = 0; probe < bucket_count; ++probe)
            {
                const std::size_t slot = (index + probe) % bucket_count;
                auto& bucket = this->buckets_[slot];

                if (bucket.is_empty() || bucket.contains(key))
                {
                    return bucket.insert(key, value);
                }
            }
            return false;
        }

        bool lookup(const Key& key, Value& value)
        {
            const std::size_t bucket_count = this->buckets_.size();
            if (bucket_count == 0)
            {
                return false;
            }

            std::size_t index = this->start_slot(key);
            for (std::size_t probe = 0; probe < bucket_count; ++probe)
            {
                const std::size_t slot = (index + probe) % bucket_count;
                auto& bucket = this->buckets_[slot];

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

        bool update(Key key, Value value)
        {
            return insert(key, value);
        }

        bool remove(const Key& key)
        {
            const std::size_t bucket_count = this->buckets_.size();
            if (bucket_count == 0)
            {
                return false;
            }

            std::size_t index = this->start_slot(key);
            for (std::size_t probe = 0; probe < bucket_count; ++probe)
            {
                const std::size_t slot = (index + probe) % bucket_count;
                auto& bucket = this->buckets_[slot];

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
