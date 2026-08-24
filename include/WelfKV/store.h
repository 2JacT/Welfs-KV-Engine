#pragma once
#include <WelfKV/common.h>

/*
Store class represents the key-value store. It takes a template parameter Config that contains the types for the key, value, hash function, and table, as well as other dependent types.
*/
template<typename Config>
class Store
{
    private:
        typename Config::table_type table_;

    public:
        using Key = typename Config::Key;
        using Value = typename Config::Value;
        using node_type = typename Config::node_type;
        using bucket_type = typename Config::bucket_type;
        using hash_function_type = typename Config::hash_function_type;
        using table_type = typename Config::table_type;

        static constexpr std::size_t key_size = Config::key_size;
        static constexpr std::size_t value_size = Config::value_size;

        explicit Store(std::size_t num_buckets, uint64_t seed = 0)
            : table_(num_buckets, seed)
        {
        }

        bool insert(Key key, Value value)
        {
            return table_.insert(key, value);
        }
        bool upsert(Key key, Value value)
        {
            return table_.update(key, value);
        }
        bool lookup(const Key& key, Value& value)
        {
            return table_.lookup(key, value);
        }
        bool remove(const Key& key)
        {
            return table_.remove(key);
        }
        bool resize()
        {
            return false;
        }
};
