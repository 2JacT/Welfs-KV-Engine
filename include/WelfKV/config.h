#pragma once
#include <WelfKV/common.h>
#include <WelfKV/node.h>
#include <WelfKV/bucket.h>
#include <WelfKV/hashfunctions.h>
#include <WelfKV/strategy.h>

template<
    std::size_t KeySize = default_key_size,
    std::size_t ValueSize = default_value_size,
    template<typename, typename, typename, typename> class StrategyTemplate = linear_probing_strategy,
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
    using strategy_type = StrategyTemplate<Key, Value, bucket_type, hash_function_type>;

    static_assert(HashTableStrategy<strategy_type>);
    static_assert(
        !strategy_type::uses_probing || bucket_type::is_addressed,
        "Probing strategies require an addressed bucket type");
};

// Set the config here:
using MyConfig = store_config<8, 64>;
