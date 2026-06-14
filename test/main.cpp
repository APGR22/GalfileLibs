#include <memory>
#include <iostream>
#include "../filesystem/filesystem.hpp"
#include "../tagsystem/tagsystem.hpp"

int main()
{
    auto file = galfile::filesystem::file::create_new(
        "test/example/test.txt",
        "custom"
    );
    auto filesystem = galfile::filesystem::Filesystem();

    std::weak_ptr<galfile::filesystem::folder::Folder> parent_folder_ptr;

    auto folder_ptr = filesystem.mkdirs("/media/linux/game");
    if (auto shared_folder_ptr = folder_ptr.lock())
    {
        std::cout << shared_folder_ptr->get_name() << std::endl;

        parent_folder_ptr = shared_folder_ptr->get_parent();
    }

    folder_ptr = filesystem.cd("media/linux");
    if (auto shared_folder_ptr = folder_ptr.lock())
    {
        std::cout << shared_folder_ptr->get_name() << std::endl;

        parent_folder_ptr = shared_folder_ptr->get_parent();
    }

    filesystem.rmdirs("/media/linux");

    std::cout << filesystem.pwd() << std::endl;

    filesystem.rmdirs("/");

    std::cout << filesystem.pwd() << std::endl;

    std::cout
    << "Is dir \"/media/linux\" exists: "
    << filesystem.isdir("/media/linux")
    << std::endl;

    return 0;
}
