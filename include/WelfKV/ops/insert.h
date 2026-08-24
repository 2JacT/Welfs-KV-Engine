#pragma once
#include <WelfKV/table.h>

template<typename Key, typename Value, typename BucketType, typename HashFn>
using LinearProbe_Insert = linear_probing_table<Key, Value, BucketType, HashFn>;
