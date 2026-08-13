#pragma once
#include <WelfKV/strategy.h>

template<typename Key, typename Value, typename StoreType>
using LinearProbe_Insert = linear_probing_strategy<Key, Value, StoreType>;
