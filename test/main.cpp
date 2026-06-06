#include "../filesystem/file/file.hpp"
#include "../helpers/checksum.hpp"
#include <iostream>

int main()
{
    auto file = galfile::filesystem::file::open_existing("test/example/test.txt");

    std::cout << "Opened: " << file.is_opened() << std::endl;

    file.write_set(0, '0', 1024);

    unsigned char write_buffer[] = "wong saya suka kok";
    file.write(0, write_buffer, sizeof(write_buffer)-1);

    unsigned char read_buffer[sizeof(write_buffer)-1];
    memset(read_buffer, 0, sizeof(read_buffer));

    file.read(0, read_buffer, sizeof(read_buffer));

    for (uint64_t index = 0; index < sizeof(read_buffer); index++)
    {
        std::cout << read_buffer[index];
    }
    std::cout << std::endl;

    std::cout << file.read_c(0) << std::endl;

    std::string checksum = galfile::helpers::checksum(read_buffer, sizeof(read_buffer));
    std::cout << checksum << std::endl;

    return 0;
}