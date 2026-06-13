#include "../filesystem/filesystem.hpp"
#include "../tagsystem/tagsystem.hpp"
#include <memory>
#include <iostream>

int main()
{
    auto file = galfile::filesystem::file::create_new("test/example/test.txt", "custom");
    auto filesystem = galfile::filesystem::Filesystem();

    auto folder_ptr = filesystem.mkdirs("/media/linux/game");
    if (!folder_ptr.expired())
    {
        auto shared_folder_ptr = folder_ptr.lock();
        std::cout << shared_folder_ptr->get_name() << std::endl;
    }

    folder_ptr = filesystem.cd("media");
    if (!folder_ptr.expired())
    {
        auto shared_folder_ptr = folder_ptr.lock();
        std::cout << shared_folder_ptr->get_name() << std::endl;
    }

    return 0;
}