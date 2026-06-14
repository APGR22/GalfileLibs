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

    auto cp_folder_ptr = filesystem.cpdir("/media/linux/game", "/");
    if (auto shared_cp_folder_ptr = cp_folder_ptr.lock())
    {
        std::cout
        << "Success to copy dir: "
        << shared_cp_folder_ptr->get_name()
        << std::endl;

        parent_folder_ptr = shared_cp_folder_ptr->get_parent();
    }

    std::cout
    << "Is dir \"/game\" exists: "
    << filesystem.isdir("/game")
    << std::endl;

    return 0;
}
