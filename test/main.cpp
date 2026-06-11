#include "../filesystem/filesystem.hpp"
#include <iostream>

int main()
{
    auto file = galfile::filesystem::file::create_new("test/example/test.txt", "custom");

    unsigned char text[] = "hello world";

    file.write(0, text, sizeof(text)-1);

    int c = file.read_c(sizeof(text)-1 - 4);

    printf("c = %c\n", c);

    return 0;
}