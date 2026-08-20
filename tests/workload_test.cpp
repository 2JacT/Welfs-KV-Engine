#include <WelfKV/config.h>
#include <WelfKV/store.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <random>
#include <string>
#include <vector>

#include "workload_config.h"

#ifndef WORKLOAD_CONFIG_PATH
#define WORKLOAD_CONFIG_PATH "tests/config/workload.cfg"
#endif

namespace
{

std::array<std::byte, MyConfig::key_size> make_key_bytes(std::uint64_t id)
{
    std::array<std::byte, MyConfig::key_size> key_bytes{};
    std::memcpy(key_bytes.data(), &id, sizeof(id));
    return key_bytes;
}

std::array<std::byte, MyConfig::value_size> make_value_bytes(std::uint64_t id)
{
    std::array<std::byte, MyConfig::value_size> value_bytes{};
    std::memcpy(value_bytes.data(), &id, sizeof(id));
    return value_bytes;
}

struct workload_result
{
    std::size_t lookup_ops = 0;
    std::size_t insert_ops = 0;
    std::size_t successful_inserts = 0;
    std::size_t successful_lookups = 0;
};

workload_result run_workload(const workload_config& config)
{
    Store<MyConfig> store(config.num_buckets);
    std::vector<std::uint64_t> inserted_keys;
    inserted_keys.reserve(config.operations);

    const std::size_t lookup_ops =
        static_cast<std::size_t>(std::llround(config.operations * config.lookup_ratio));
    const std::size_t insert_ops = config.operations - lookup_ops;

    std::vector<bool> schedule(config.operations, false);
    std::fill(schedule.begin(), schedule.begin() + lookup_ops, true);

    std::mt19937 rng(0xC0FFEE);
    std::shuffle(schedule.begin(), schedule.end(), rng);

    workload_result result{};
    result.lookup_ops = lookup_ops;
    result.insert_ops = insert_ops;

    std::uint64_t next_key = 1;

    for (const bool is_lookup : schedule)
    {
        if (is_lookup && !inserted_keys.empty())
        {
            std::uniform_int_distribution<std::size_t> key_dist(0, inserted_keys.size() - 1);
            const std::uint64_t key_id = inserted_keys[key_dist(rng)];
            auto key_bytes = make_key_bytes(key_id);
            MyConfig::Key key(key_bytes);

            std::array<std::byte, MyConfig::value_size> out_bytes{};
            MyConfig::Value out_value(out_bytes);

            if (store.lookup(key, out_value))
            {
                ++result.successful_lookups;
            }
        }
        else
        {
            const std::uint64_t key_id = next_key++;
            auto key_bytes = make_key_bytes(key_id);
            auto value_bytes = make_value_bytes(key_id);
            MyConfig::Key key(key_bytes);
            MyConfig::Value value(value_bytes);

            if (store.insert(key, value))
            {
                inserted_keys.push_back(key_id);
                ++result.successful_inserts;
            }
        }
    }

    return result;
}

} // namespace

TEST(WorkloadConfig, ReadsSplitAndOperationCount)
{
    const workload_config config = load_workload_config(WORKLOAD_CONFIG_PATH);

    EXPECT_DOUBLE_EQ(config.lookup_ratio, 0.8);
    EXPECT_DOUBLE_EQ(config.insert_ratio, 0.2);
    EXPECT_EQ(config.operations, 10000U);
    EXPECT_EQ(config.num_buckets, 1024U);
}

TEST(WorkloadConfig, RunsConfiguredSplit)
{
    const workload_config config = load_workload_config(WORKLOAD_CONFIG_PATH);
    const workload_result result = run_workload(config);

    EXPECT_EQ(result.lookup_ops + result.insert_ops, config.operations);
    EXPECT_EQ(result.lookup_ops, 8000U);
    EXPECT_EQ(result.insert_ops, 2000U);
    EXPECT_GT(result.successful_inserts, 0U);
    EXPECT_GT(result.successful_lookups, 0U);
}
