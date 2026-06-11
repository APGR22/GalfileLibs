#include "../filesystem/filesystem.hpp"
#include <iostream>

int main()
{
    auto filesystem = galfile::filesystem::Filesystem();
    auto *folder = filesystem.mkdirs("/media/linux/gamer");

    std::cout << folder << std::endl;
    if (folder)
    {
        std::cout << folder->get_name() << std::endl;
    }

    folder = filesystem.cd("./media/linux");

    std::cout << folder << std::endl;
    if (folder)
    {
        std::cout << folder->get_name() << std::endl;
    }

    return 0;
}