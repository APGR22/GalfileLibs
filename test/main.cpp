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

    auto file_ptr = filesystem.mkfile("/media/file.txt", "test/example/test.txt");
    if (auto shared_file_ptr = file_ptr.lock())
    {
        std::cout
        << "Success to create a file: "
        << shared_file_ptr->get_name()
        << std::endl;
    }

    auto mv_file_ptr = filesystem.mvfile("/media/file.txt", "/");
    if (auto shared_mv_file_ptr = mv_file_ptr.lock())
    {
        std::cout
        << "Success to move a file: "
        << shared_mv_file_ptr->get_name()
        << std::endl;
    }

    std::cout
    << "Is \"/media/file.txt\" file exists: "
    << filesystem.isfile("/media/file.txt")
    << std::endl;

    std::cout
    << "Is \"/file.txt\" file exists: "
    << filesystem.isfile("/file.txt")
    << std::endl;

    std::cout
    << "Is old file_ptr still exists: "
    << !file_ptr.expired()
    << std::endl;

    return 0;
}
