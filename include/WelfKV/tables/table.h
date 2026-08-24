#pragma once
//Include all table implementations <Start>
#include <WelfKV/tables/probing/linear_probing_table.h>
#include <WelfKV/tables/cuckoo_table.h>
//Include all table implementations <End>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

template<typename Table>
concept HashTable = requires {
    typename Table::key_type;
    typename Table::value_type;
    typename Table::bucket_type;
    requires std::convertible_to<decltype(Table::uses_probing), bool>;
} && requires(
    typename Table::key_type key,
    typename Table::value_type value,
    const typename Table::key_type& lookup_key,
    typename Table::value_type& out_value) {
    { std::declval<Table&>().insert(key, value) } -> std::same_as<bool>;
    { std::declval<Table&>().lookup(lookup_key, out_value) } -> std::same_as<bool>;
    { std::declval<Table&>().update(key, value) } -> std::same_as<bool>;
    { std::declval<Table&>().remove(lookup_key) } -> std::same_as<bool>;
};
