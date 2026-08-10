#pragma once
#include <WelfKV/common.h>
// NodeBytes is total inline payload size for one entry; key and value each get half (rounded down for key).
template<std::size_t NodeBytes>
class node
{
    public:
        static constexpr std::size_t max_key_bytes = NodeBytes / 2;
        static constexpr std::size_t max_value_bytes = NodeBytes - max_key_bytes;

        static bool fits(Key key, Value value)
        {
            return key.size_bytes() <= max_key_bytes && value.size_bytes() <= max_value_bytes;
        }

        node(Key key, Value value)
        {
            assign_key(key);
            assign_value(value);
        }

        const Key& get_key() const { return key_view_; }
        const Value& get_value() const { return value_view_; }

        bool set_value(Value value)
        {
            if (value.size_bytes() > max_value_bytes)
            {
                return false;
            }
            assign_value(value);
            return true;
        }

    private:
        std::array<std::byte, max_key_bytes> key_storage_{};
        std::size_t key_size_ = 0;
        std::array<std::byte, max_value_bytes> value_storage_{};
        std::size_t value_size_ = 0;
        Key key_view_{};
        Value value_view_{};

        void assign_key(Key key)
        {
            key_size_ = std::min(key.size_bytes(), max_key_bytes);
            if (key_size_ > 0)
            {
                std::memcpy(key_storage_.data(), key.data(), key_size_);
            }
            key_view_ = Key(key_storage_.data(), key_size_);
        }

        void assign_value(Value value)
        {
            value_size_ = std::min(value.size_bytes(), max_value_bytes);
            if (value_size_ > 0)
            {
                std::memcpy(value_storage_.data(), value.data(), value_size_);
            }
            value_view_ = Value(value_storage_.data(), value_size_);
        }
};
