#pragma once
#include <WelfKV/common.h>
#include <WelfKV/node.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <optional>
#include <list>


// Bucket represents the slot a key hashes to and the building block of our key value stores.
// While the probing algorithm defines how we traverse buckets to check for an open slot, the bucket defines when it is full or empty.
class bucket
{
    public:
        virtual bool insert(Key key, Value value) = 0;
        virtual bool remove(const Key& key) = 0;
        virtual bool get(const Key& key, Value& value) = 0;
        virtual bool contains(const Key& key) = 0;
        virtual bool is_empty() = 0;
        virtual bool is_full() = 0;
};

template<std::size_t NodeBytes>
class chained_bucket : public bucket
{
    protected:
        std::list<node<NodeBytes>> nodes_;
    public:
        bool insert(Key key, Value value) override
        {
            if (!node<NodeBytes>::fits(key, value))
            {
                return false;
            }
            for (auto& n : nodes_)
            {
                if (n.get_key() == key)
                {
                    return n.set_value(value);
                }
            }
            if (!is_full())
            {
                nodes_.push_back(node<NodeBytes>(key, value));
                return true;
            }
            return false;
        }
        bool remove(const Key& key) override
        {
            for (auto it = nodes_.begin(); it != nodes_.end(); ++it)
            {
                if (it->get_key() == key)
                {
                    nodes_.erase(it);
                    return true;
                }
            }
            return false;
        }
        bool get(const Key& key, Value& value) override
        {
            for (const auto& n : nodes_)
            {
                if (n.get_key() == key)
                {
                    value = n.get_value();
                    return true;
                }
            }
            return false;
        }
        bool contains(const Key& key) override
        {
            for (const auto& n : nodes_)
            {
                if (n.get_key() == key)
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
};

template<std::size_t NodeBytes>
class addressed_bucket : public bucket
{
    protected:
        std::optional<node<NodeBytes>> node_;
    public:
        bool insert(Key key, Value value) override
        {
            if (!node<NodeBytes>::fits(key, value))
            {
                return false;
            }
            if (!node_.has_value())
            {
                node_.emplace(key, value);
                return true;
            }
            if (node_->get_key() == key)
            {
                return node_->set_value(value);
            }
            return false;
        }
        bool remove(const Key& key) override
        {
            if (node_.has_value() && node_->get_key() == key)
            {
                node_.reset();
                return true;
            }
            return false;
        }
        bool get(const Key& key, Value& value) override
        {
            if (node_.has_value() && node_->get_key() == key)
            {
                value = node_->get_value();
                return true;
            }
            return false;
        }
        bool contains(const Key& key) override
        {
            return node_.has_value() && node_->get_key() == key;
        }
        bool is_empty() override
        {
            return !node_.has_value();
        }
        bool is_full() override
        {
            return node_.has_value();
        }
};

inline constexpr std::size_t default_node_bytes = 128;

using default_node = node<default_node_bytes>;
using default_chained_bucket = chained_bucket<default_node_bytes>;
using default_addressed_bucket = addressed_bucket<default_node_bytes>;
