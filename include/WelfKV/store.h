#pragma once
#include <vector>
#include <WelfKV/bucket.h>
#include <WelfKV/common.h>
#include <WelfKV/hashfunctions.h>
#include <WelfKV/strategy.h>
#include <xxhash.h>

template<typename Config>
class Store
{
    public:
        using Key = typename Config::Key;
        using Value = typename Config::Value;
        using node_type = typename Config::node_type;
        using bucket_type = typename Config::bucket_type;
        using strategy_type = typename Config::template strategy_for<Store<Config>>;
        using hash_function_type = HashFunction<Config>;

        static constexpr std::size_t key_size = Config::key_size;
        static constexpr std::size_t value_size = Config::value_size;

        std::vector<bucket_type> buckets;

        explicit Store(std::size_t num_buckets) : buckets(num_buckets) {}

        uint64_t hash(Key key) const
        {
            //#TODO: Remove hard coded hash function and figure out how to make it customizable (compile time vs runtime config)
            XXH64_hash_t digest = XXH3_64bits(key.data(), key.size_bytes());
            return digest;
        }

        bool insert(Key key, Value value)
        {
            return strategy_type::insert(key, value, *this);
        }
        bool upsert(Key key, Value value)
        {
            return strategy_type::update(key, value, *this);
        }
        bool lookup(const Key& key, Value& value)
        {
            return strategy_type::lookup(key, value, *this);
        }
        bool remove(const Key& key)
        {
            return strategy_type::remove(key, *this);
        }
        bool resize()
        {
            return false;
        }
};
