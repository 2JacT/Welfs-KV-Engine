#pragma once
#include <vector>
#include <WelfKV/common.h>

/*
Store class represents the key-value store. It takes a template parameter Config that contains the types for the key, value, hash function, and strategy, as well as other dependent types.
*/
template<typename Config>
class Store
{
    public:
        using Key = typename Config::Key;
        using Value = typename Config::Value;
        using node_type = typename Config::node_type;
        using bucket_type = typename Config::bucket_type;
        using hash_function_type = typename Config::hash_function_type;
        using strategy_type = typename Config::strategy_type;

        static constexpr std::size_t key_size = Config::key_size;
        static constexpr std::size_t value_size = Config::value_size;

    protected:
        std::vector<bucket_type> buckets;
    public:
    
        explicit Store(std::size_t num_buckets) : buckets(num_buckets) {}

        bool insert(Key key, Value value)
        {
            return strategy_type::insert(key, value, buckets);
        }
        bool upsert(Key key, Value value)
        {
            return strategy_type::update(key, value, buckets);
        }
        bool lookup(const Key& key, Value& value)
        {
            return strategy_type::lookup(key, value, buckets);
        }
        bool remove(const Key& key)
        {
            return strategy_type::remove(key, buckets);
        }
        bool resize()
        {
            return false;
        }
};
