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

    auto mv_folder_ptr = filesystem.mvdir("/media/linux/game", "/");
    if (auto shared_mv_folder_ptr = mv_folder_ptr.lock())
    {
        std::cout
        << "Success to move dir: "
        << shared_mv_folder_ptr->get_name()
        << std::endl;
    }

    std::cout
    << "Is dir \"/game\" exists: "
    << filesystem.isdir("/game")
    << std::endl;

    std::cout
    << "Is dir \"/media/linux/game\" exists: "
    << filesystem.isdir("/media/linux/game")
    << std::endl;

    std::cout
    << "Is old folder_ptr still exists: "
    << !folder_ptr.expired()
    << std::endl;

    return 0;
}
