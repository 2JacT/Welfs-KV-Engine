#pragma once
#include <WelfKV/common.h>
#include <WelfKV/node.h>
#include <WelfKV/bucket.h>
#include <WelfKV/strategy.h>

template<
    std::size_t KeySize = default_key_size,
    std::size_t ValueSize = default_value_size,
    template<typename, typename, typename> class StrategyTemplate = linear_probing_strategy,
    template<typename, typename, typename> class BucketTemplate = addressed_bucket>
struct store_config
{
    static constexpr std::size_t key_size = KeySize;
    static constexpr std::size_t value_size = ValueSize;

    using Key = ::Key<KeySize>;
    using Value = ::Value<ValueSize>;
    using node_type = node<KeySize, ValueSize>;
    using bucket_type = BucketTemplate<Key, Value, node_type>;

    template<typename StoreType>
    using strategy_for = StrategyTemplate<Key, Value, StoreType>;

    static_assert(
        !StrategyTemplate<Key, Value, int>::uses_probing || bucket_type::is_addressed,
        "Probing strategies require an addressed bucket type");
};

// Set the config here:
using MyConfig = store_config<8, 64>;
