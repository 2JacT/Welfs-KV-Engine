#pragma once
#include <WelfKV/common.h>
#include <array>
#include <cstring>

// node is one storage unit for a single key/value pair.
// It defines the fixed byte layout for that pair; buckets collect nodes and define slot behavior.
template<std::size_t KeySize, std::size_t ValueSize>
class node
{
    public:
        using key_type = Key<KeySize>;
        using value_type = Value<ValueSize>;

        static constexpr std::size_t max_key_bytes = KeySize;
        static constexpr std::size_t max_value_bytes = ValueSize;

        static bool fits(key_type key, value_type value)
        {
            return key.size_bytes() <= max_key_bytes && value.size_bytes() <= max_value_bytes;
        }

    private:
        std::array<std::byte, KeySize> key_storage_{};
        std::array<std::byte, ValueSize> value_storage_{};
        key_type key_view_;
        value_type value_view_;

    public:
        node(key_type key, value_type value)
            : key_view_(key_storage_)
            , value_view_(value_storage_)
        {
            assign_key(key);
            assign_value(value);
        }

        const key_type& get_key() const { return key_view_; }
        const value_type& get_value() const { return value_view_; }

        bool set_value(value_type value)
        {
            if (value.size_bytes() > max_value_bytes)
            {
                return false;
            }
            assign_value(value);
            return true;
        }

    private:
        void assign_key(key_type key)
        {
            std::memcpy(key_storage_.data(), key.data(), KeySize);
            key_view_ = key_type(key_storage_);
        }

        void assign_value(value_type value)
        {
            std::memcpy(value_storage_.data(), value.data(), ValueSize);
            value_view_ = value_type(value_storage_);
        }
};
