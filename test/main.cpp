#include "../helpers/hex.hpp"
#include <iostream>

int main()
{
    for (int val = 0; val < 256; val++)
    {
        std::cout << "[" << val << "]: " << galfile::helpers::hex(val) << std::endl;
    }

    return 0;
}