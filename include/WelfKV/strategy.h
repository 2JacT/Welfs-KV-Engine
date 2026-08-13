#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>

template<typename Strategy>
concept HashTableStrategy = requires(
    typename Strategy::key_type key,
    typename Strategy::value_type value,
    typename Strategy::store_type& store,
    const typename Strategy::key_type& lookup_key,
    typename Strategy::value_type& out_value)
{
    { Strategy::insert(key, value, store) } -> std::same_as<bool>;
    { Strategy::lookup(lookup_key, out_value, store) } -> std::same_as<bool>;
    { Strategy::update(key, value, store) } -> std::same_as<bool>;
    { Strategy::remove(lookup_key, store) } -> std::same_as<bool>;
    requires std::convertible_to<decltype(Strategy::uses_probing), bool>;
};

template<typename Key, typename Value, typename StoreType>
struct Probing_Strategy
{
    using key_type = Key;
    using value_type = Value;
    using store_type = StoreType;

    protected:
        static std::size_t start_slot(const StoreType& store, Key key)
        {
            return static_cast<std::size_t>(store.hash(key) % store.buckets.size());
        }
};

template<typename Key, typename Value, typename StoreType>
struct linear_probing_strategy : Probing_Strategy<Key, Value, StoreType>
{
    using key_type = Key;
    using value_type = Value;
    using store_type = StoreType;

    static constexpr bool uses_probing = true;

    static bool insert(Key key, Value value, StoreType& store)
    {
        const std::size_t bucket_count = store.buckets.size();
        if (bucket_count == 0)
        {
            return false;
        }

        std::size_t index = Probing_Strategy<Key, Value, StoreType>::start_slot(store, key);
        for (std::size_t probe = 0; probe < bucket_count; ++probe)
        {
            const std::size_t slot = (index + probe) % bucket_count;
            auto& bucket = store.buckets[slot];

            if (bucket.is_empty() || bucket.contains(key))
            {
                return bucket.insert(key, value);
            }
        }
        return false;
    }

    static bool lookup(const Key& key, Value& value, StoreType& store)
    {
        const std::size_t bucket_count = store.buckets.size();
        if (bucket_count == 0)
        {
            return false;
        }

        std::size_t index = Probing_Strategy<Key, Value, StoreType>::start_slot(store, key);
        for (std::size_t probe = 0; probe < bucket_count; ++probe)
        {
            const std::size_t slot = (index + probe) % bucket_count;
            auto& bucket = store.buckets[slot];

            if (bucket.is_empty())
            {
                return false;
            }
            if (bucket.contains(key))
            {
                return bucket.get(key, value);
            }
        }
        return false;
    }

    static bool update(Key key, Value value, StoreType& store)
    {
        return insert(key, value, store);
    }

    static bool remove(const Key& key, StoreType& store)
    {
        const std::size_t bucket_count = store.buckets.size();
        if (bucket_count == 0)
        {
            return false;
        }

        std::size_t index = Probing_Strategy<Key, Value, StoreType>::start_slot(store, key);
        for (std::size_t probe = 0; probe < bucket_count; ++probe)
        {
            const std::size_t slot = (index + probe) % bucket_count;
            auto& bucket = store.buckets[slot];

            if (bucket.is_empty())
            {
                return false;
            }
            if (bucket.contains(key))
            {
                return bucket.remove(key);
            }
        }
        return false;
    }
};
