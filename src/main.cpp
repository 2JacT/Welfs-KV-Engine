#include <WelfKV/config.h>
#include <WelfKV/store.h>
#include <array>
#include <iostream>

int main()
{
    std::array<std::byte, MyConfig::key_size> key_bytes = {
        std::byte{1},
        std::byte{2},
        std::byte{3},
    };
    std::array<std::byte, MyConfig::value_size> value_bytes = {
        std::byte{42},
    };

    MyConfig::Key key(key_bytes);
    MyConfig::Value value(value_bytes);

    Store<MyConfig> store(8);
    const bool inserted = store.insert(key, value);

    MyConfig::Value looked_up(value_bytes);
    const bool found = store.lookup(key, looked_up);

    std::cout << "inserted: " << std::boolalpha << inserted << std::endl;
    std::cout << "found: " << found << std::endl;
}
