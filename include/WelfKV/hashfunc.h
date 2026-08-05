#pragma once
#include <WelfKV/common.h>

class HashFunc
{
    public:
        virtual uint64_t Hash(Key key) = 0;
};

