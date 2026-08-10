#pragma once

#include <WelfKV/common.h>

class Store
{
    public:
        Store();
        virtual bool Put(Key key, Value value) = 0;
        virtual bool Get(Key key, Value* value) = 0;
        virtual bool Remove(Key key) = 0;
        virtual bool Contains(Key key) const = 0;
        virtual std::size_t Size() const = 0;
        virtual void Clear() = 0;
};

class ChainedStore :: Store
{
    protected:

}
class AddressedStore :: Store
{

}
