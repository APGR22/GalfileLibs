#include <memory>
#include <iostream>
#include "../filesystem/filesystem.hpp"
#include "../tagsystem/tagsystem.hpp"

int main()
{
    auto filesystem = galfile::filesystem::Filesystem();

    auto folder = filesystem.mkdirs("/main/linux");
    auto file = filesystem.mkfile("/main/linux/filetest.txt", "test/example/test.txt");

    std::cout << folder->get_name() << '\n';
    std::cout << file->get_name() << '\n';

    unsigned char text[] = "Apa saja";
    file->write(0, text, sizeof(text)-1);

    return 0;
}
