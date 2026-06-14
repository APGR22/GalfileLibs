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

    auto cp_file_ptr = filesystem.cpfile("/media/file.txt", "/");
    if (auto shared_cp_file_ptr = cp_file_ptr.lock())
    {
        std::cout
        << "Success to create a file: "
        << shared_cp_file_ptr->get_name()
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

    return 0;
}
