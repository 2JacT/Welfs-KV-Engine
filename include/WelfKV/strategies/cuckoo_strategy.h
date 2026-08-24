#pragma once
#include <WelfKV/common.h>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

//#TODO: Refactor strategy to use instantiation in store.h so we can cache certain values like buckets.size() and is_valid() cehcks
// Cuckoo hashing uses two hash positions per key. Table size must be even; each half is one hash range.
template<typename Key, typename Value, typename BucketType, typename HashFn>
struct cuckoo_strategy
{
    using key_type = Key;
    using value_type = Value;
    using bucket_type = BucketType;
    using hash_function_type = HashFn;

    static constexpr bool uses_probing = false;
    static constexpr bool requires_addressed_bucket = true;

    private:
        static std::size_t half_size(const std::vector<BucketType>& buckets)
        {
            return buckets.size() / 2;
        }

        static std::size_t hash1_index(const std::vector<BucketType>& buckets, Key key)
        {
            return static_cast<std::size_t>(HashFn::hash(key) % half_size(buckets));
        }

        static std::size_t hash2_index(const std::vector<BucketType>& buckets, Key key)
        {
            const std::size_t half = half_size(buckets);
            const uint64_t digest = HashFn::hash(key);
            const uint64_t secondary = (digest >> 32) ^ (digest * 0x9e3779b97f4a7c15ULL);
            return half + static_cast<std::size_t>(secondary % half);
        }

        static std::size_t alternate_index(
            const std::vector<BucketType>& buckets,
            std::size_t current_index,
            Key key)
        {
            const std::size_t first = hash1_index(buckets, key);
            return current_index == first ? hash2_index(buckets, key) : first;
        }

        static bool valid_table(const std::vector<BucketType>& buckets)
        {
            return buckets.size() >= 2 && buckets.size() % 2 == 0;
        }

    public:
        static bool insert(Key key, Value value, std::vector<BucketType>& buckets)
        {
            if (!valid_table(buckets))
            {
                return false;
            }

            const std::size_t first_index = hash1_index(buckets, key);
            if (buckets[first_index].insert(key, value))
            {
                return true;
            }

            const std::size_t second_index = hash2_index(buckets, key);
            if (buckets[second_index].insert(key, value))
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

            const std::size_t max_kicks = static_cast<std::size_t>(
                5.0 * std::log2(static_cast<double>(buckets.size())));
            for (std::size_t kick = 0; kick < max_kicks; ++kick)
            {
                auto& bucket = buckets[index];
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
                index = alternate_index(buckets, index, current_key);
            }

            return false;
        }

        static bool lookup(const Key& key, Value& value, std::vector<BucketType>& buckets)
        {
            if (!valid_table(buckets))
            {
                return false;
            }

            if (buckets[hash1_index(buckets, key)].get(key, value))
            {
                return true;
            }
            return buckets[hash2_index(buckets, key)].get(key, value);
        }

        static bool update(Key key, Value value, std::vector<BucketType>& buckets)
        {
            return insert(key, value, buckets);
        }

        static bool remove(const Key& key, std::vector<BucketType>& buckets)
        {
            if (!valid_table(buckets))
            {
                return false;
            }

            if (buckets[hash1_index(buckets, key)].remove(key))
            {
                return true;
            }
            return buckets[hash2_index(buckets, key)].remove(key);
        }
};
