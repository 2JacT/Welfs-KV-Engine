// common.h
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

inline constexpr std::size_t default_key_size = 8;
inline constexpr std::size_t default_value_size = 64;

template<std::size_t KeySize = default_key_size>
using Key = std::span<const std::byte, KeySize>;

template<std::size_t ValueSize = default_value_size>
using Value = std::span<const std::byte, ValueSize>;

template<std::size_t N>
inline bool bytes_equal(std::span<const std::byte, N> a, std::span<const std::byte, N> b)
{
    return std::equal(a.begin(), a.end(), b.begin());
}
