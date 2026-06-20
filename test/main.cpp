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

    file->write_str(0, "Apa saja lah");

    return 0;
}
