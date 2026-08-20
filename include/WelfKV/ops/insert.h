#pragma once
#include <WelfKV/strategy.h>

template<typename Key, typename Value, typename BucketType, typename HashFn>
using LinearProbe_Insert = linear_probing_strategy<Key, Value, BucketType, HashFn>;
