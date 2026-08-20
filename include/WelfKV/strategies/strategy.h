#pragma once
//Include all strategy implementations <Start>
#include <WelfKV/strategies/probing/linear_probing_strategy.h>
//Include all strategy implementations <End>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>
/*
 HashTableStrategy concept defines the interface that a strategy must implement to be used within a store.
*/
template<typename Strategy>
concept HashTableStrategy = requires(
    typename Strategy::key_type key,
    typename Strategy::value_type value,
    std::vector<typename Strategy::bucket_type>& buckets,
    const typename Strategy::key_type& lookup_key,
    typename Strategy::value_type& out_value)
{
    { Strategy::insert(key, value, buckets) } -> std::same_as<bool>;
    { Strategy::lookup(lookup_key, out_value, buckets) } -> std::same_as<bool>;
    { Strategy::update(key, value, buckets) } -> std::same_as<bool>;
    { Strategy::remove(lookup_key, buckets) } -> std::same_as<bool>;
    requires std::convertible_to<decltype(Strategy::uses_probing), bool>;
};
