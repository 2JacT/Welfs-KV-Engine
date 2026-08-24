#pragma once
#include <WelfKV/common.h>
#include <WelfKV/node.h>
#include <WelfKV/bucket.h>
#include <WelfKV/hashfunctions.h>
#include <WelfKV/table.h>

template<
    std::size_t KeySize = default_key_size,
    std::size_t ValueSize = default_value_size,
    template<typename, typename, typename, typename> class TableTemplate = linear_probing_table,
    template<typename, typename, typename> class BucketTemplate = addressed_bucket,
    template<typename> class HashFunctionTemplate = xxhash_function>
struct store_config
{
    static constexpr std::size_t key_size = KeySize;
    static constexpr std::size_t value_size = ValueSize;

    using Key = ::Key<KeySize>;
    using Value = ::Value<ValueSize>;
    using node_type = node<KeySize, ValueSize>;
    using bucket_type = BucketTemplate<Key, Value, node_type>;
    using hash_function_type = HashFunctionTemplate<Key>;
    using table_type = TableTemplate<Key, Value, bucket_type, hash_function_type>;

    static_assert(HashTable<table_type>);
    static_assert(
        (!table_type::uses_probing && !table_type::requires_addressed_bucket)
            || bucket_type::is_addressed,
        "This table requires an addressed bucket type");
};

// Set the config here:
using MyConfig = store_config<8, 64>;
