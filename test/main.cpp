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
    if (!folder_ptr.expired())
    {
        auto shared_folder_ptr = folder_ptr.lock();
        std::cout << shared_folder_ptr->get_name() << std::endl;

        parent_folder_ptr = shared_folder_ptr->get_parent();
    }

    folder_ptr = filesystem.cd("media");
    if (!folder_ptr.expired())
    {
        auto shared_folder_ptr = folder_ptr.lock();
        std::cout << shared_folder_ptr->get_name() << std::endl;

        parent_folder_ptr = shared_folder_ptr->get_parent();
    }

    if (!parent_folder_ptr.expired())
    {
        auto shared_parent_folder_ptr = parent_folder_ptr.lock();
        std::cout
        << "Parent: "
        << shared_parent_folder_ptr->get_name()
        << std::endl;
    }

    filesystem.rmdirs("/media/linux");
    if (!folder_ptr.expired())
    {
        std::cout << "Success to keep alive from rmdirs" << std::endl;
    }

    filesystem.rmdirs("/");
    if (folder_ptr.expired())
    {
        std::cout << "Success to remove from root" << std::endl;
    }

    return 0;
}
