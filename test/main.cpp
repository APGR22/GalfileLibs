#include "../filesystem/filesystem.hpp"
#include <iostream>

int main()
{
    auto file = galfile::filesystem::file::create_new("test/example/test.txt", "custom");

    std::cout << file.get_parent() << std::endl;

    return 0;
}