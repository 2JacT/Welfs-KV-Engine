#pragma once
#include <WelfKV/common.h>
#include <iostream>

int main()
{
    std::vector<std::byte> data = {
    std::byte{1},
    std::byte{2},
    std::byte{3}
    };

    Key key(data);
    std::cout << "Yolo" << std::endl;
}