#pragma once
#include <WelfKV/common.h>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

// Cuckoo hashing uses two hash positions per key. Table size must be even; each half is one hash range.
template<typename Key, typename Value, typename BucketType, typename HashFn>
class cuckoo_table
{
    public:
        using key_type = Key;
        using value_type = Value;
        using bucket_type = BucketType;
        using hash_function_type = HashFn;

        static constexpr bool uses_probing = false;
        static constexpr bool requires_addressed_bucket = true;

    private:
        std::vector<BucketType> buckets_;
        uint64_t seed_;
        uint64_t seed2_;
        std::size_t half_size_;
        std::size_t max_kicks_;
        bool valid_table_;

    public:
        explicit cuckoo_table(std::size_t num_buckets, uint64_t seed = 0)
            : buckets_(num_buckets)
            , seed_(seed)
            , seed2_(seed ^ 0x9e3779b97f4a7c15ULL)
            , half_size_(num_buckets / 2)
            , max_kicks_(static_cast<std::size_t>(
                  5.0 * std::log2(static_cast<double>(num_buckets > 0 ? num_buckets : 1))))
            , valid_table_(num_buckets >= 2 && num_buckets % 2 == 0)
        {
        }

        bool insert(Key key, Value value)
        {
            if (!valid_table_)
            {
                return false;
            }

            const std::size_t first_index = hash1_index(key);
            if (buckets_[first_index].insert(key, value))
            {
                return true;
            }

            const std::size_t second_index = hash2_index(key);
            if (buckets_[second_index].insert(key, value))
            {
                return true;
            }

            std::array<std::byte, key.size_bytes()> current_key_storage{};
            std::array<std::byte, value.size_bytes()> current_value_storage{};
            std::memcpy(current_key_storage.data(), key.data(), key.size_bytes());
            std::memcpy(current_value_storage.data(), value.data(), value.size_bytes());

            Key current_key(current_key_storage);
            Value current_value(current_value_storage);
            std::size_t index = first_index;

            for (std::size_t kick = 0; kick < max_kicks_; ++kick)
            {
                auto& bucket = buckets_[index];
                if (bucket.insert(current_key, current_value))
                {
                    return true;
                }

                std::array<std::byte, current_key.size_bytes()> evicted_key_storage{};
                std::array<std::byte, current_value.size_bytes()> evicted_value_storage{};
                if (!bucket.displace(
                        current_key,
                        current_value,
                        evicted_key_storage,
                        evicted_value_storage))
                {
                    return false;
                }

                current_key = Key(evicted_key_storage);
                current_value = Value(evicted_value_storage);
                index = alternate_index(index, current_key);
            }

            return false;
        }

        bool lookup(const Key& key, Value& value)
        {
            if (!valid_table_)
            {
                return false;
            }

            if (buckets_[hash1_index(key)].get(key, value))
            {
                return true;
            }
            return buckets_[hash2_index(key)].get(key, value);
        }

        bool update(Key key, Value value)
        {
            return insert(key, value);
        }

        bool remove(const Key& key)
        {
            if (!valid_table_)
            {
                return false;
            }

            if (buckets_[hash1_index(key)].remove(key))
            {
                return true;
            }
            return buckets_[hash2_index(key)].remove(key);
        }

    private:
        std::size_t hash1_index(Key key) const
        {
            return static_cast<std::size_t>(HashFn::hash(key, seed_) % half_size_);
        }

        std::size_t hash2_index(Key key) const
        {
            return half_size_ + static_cast<std::size_t>(HashFn::hash(key, seed2_) % half_size_);
        }

        std::size_t alternate_index(std::size_t current_index, Key key) const
        {
            const std::size_t first = hash1_index(key);
            return current_index == first ? hash2_index(key) : first;
        }
};
