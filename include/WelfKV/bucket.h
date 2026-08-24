#pragma once
#include <WelfKV/common.h>
#include <WelfKV/node.h>
#include <list>
#include <optional>
#include <span>
#include <cstring>

// Bucket represents the slot a key hashes to and the building block of our key value stores.
// While the probing algorithm defines how we traverse buckets to check for an open slot, the bucket defines when it is full or empty.
template<typename KeyType, typename ValueType, typename NodeType>
class bucket
{
    public:
        virtual bool insert(KeyType key, ValueType value) = 0;
        virtual bool remove(const KeyType& key) = 0;
        virtual bool get(const KeyType& key, ValueType& value) = 0;
        virtual bool contains(const KeyType& key) = 0;
        virtual bool is_empty() = 0;
        virtual bool is_full() = 0;
        virtual bool displace(
            KeyType key,
            ValueType value,
            std::span<std::byte> evicted_key_out,
            std::span<std::byte> evicted_value_out) = 0;
        virtual ~bucket() = default;
};

template<typename KeyType, typename ValueType, typename NodeType>
class chained_bucket : public bucket<KeyType, ValueType, NodeType>
{
    public:
        static constexpr bool is_addressed = false;

    protected:
        std::list<NodeType> nodes_;
    public:
        bool insert(KeyType key, ValueType value) override
        {
            if (!NodeType::fits(key, value))
            {
                return false;
            }
            for (auto& n : nodes_)
            {
                if (bytes_equal(n.get_key(), key))
                {
                    return n.set_value(value);
                }
            }
            if (!is_full())
            {
                nodes_.emplace_back(key, value);
                return true;
            }
            return false;
        }
        bool remove(const KeyType& key) override
        {
            for (auto it = nodes_.begin(); it != nodes_.end(); ++it)
            {
                if (bytes_equal(it->get_key(), key))
                {
                    nodes_.erase(it);
                    return true;
                }
            }
            return false;
        }
        bool get(const KeyType& key, ValueType& value) override
        {
            for (const auto& n : nodes_)
            {
                if (bytes_equal(n.get_key(), key))
                {
                    std::memcpy(
                        const_cast<std::byte*>(value.data()),
                        n.get_value().data(),
                        value.size_bytes());
                    return true;
                }
            }
            return false;
        }
        bool contains(const KeyType& key) override
        {
            for (const auto& n : nodes_)
            {
                if (bytes_equal(n.get_key(), key))
                {
                    return true;
                }
            }
            return false;
        }
        bool is_empty() override
        {
            return nodes_.empty();
        }
        bool is_full() override
        {
            return false;
        }
        bool displace(
            KeyType key,
            ValueType value,
            std::span<std::byte> evicted_key_out,
            std::span<std::byte> evicted_value_out) override
        {
            return false;
        }
};

template<typename KeyType, typename ValueType, typename NodeType>
class addressed_bucket : public bucket<KeyType, ValueType, NodeType>
{
    public:
        static constexpr bool is_addressed = true;

    protected:
        std::optional<NodeType> node_;
    public:
        bool insert(KeyType key, ValueType value) override
        {
            if (!NodeType::fits(key, value))
            {
                return false;
            }
            if (!node_.has_value())
            {
                node_.emplace(key, value);
                return true;
            }
            if (bytes_equal(node_->get_key(), key))
            {
                return node_->set_value(value);
            }
            return false;
        }
        bool remove(const KeyType& key) override
        {
            if (node_.has_value() && bytes_equal(node_->get_key(), key))
            {
                node_.reset();
                return true;
            }
            return false;
        }
        bool get(const KeyType& key, ValueType& value) override
        {
            if (node_.has_value() && bytes_equal(node_->get_key(), key))
            {
                std::memcpy(
                    const_cast<std::byte*>(value.data()),
                    node_->get_value().data(),
                    value.size_bytes());
                return true;
            }
            return false;
        }
        bool contains(const KeyType& key) override
        {
            return node_.has_value() && bytes_equal(node_->get_key(), key);
        }
        bool is_empty() override
        {
            return !node_.has_value();
        }
        bool is_full() override
        {
            return node_.has_value();
        }
        bool displace(
            KeyType key,
            ValueType value,
            std::span<std::byte> evicted_key_out,
            std::span<std::byte> evicted_value_out) override
        {
            if (!NodeType::fits(key, value))
            {
                return false;
            }
            if (!node_.has_value())
            {
                node_.emplace(key, value);
                return true;
            }
            if (bytes_equal(node_->get_key(), key))
            {
                return node_->set_value(value);
            }
            if (evicted_key_out.size_bytes() < node_->get_key().size_bytes()
                || evicted_value_out.size_bytes() < node_->get_value().size_bytes())
            {
                return false;
            }
            std::memcpy(
                evicted_key_out.data(),
                node_->get_key().data(),
                node_->get_key().size_bytes());
            std::memcpy(
                evicted_value_out.data(),
                node_->get_value().data(),
                node_->get_value().size_bytes());
            node_.emplace(key, value);
            return true;
        }
};
