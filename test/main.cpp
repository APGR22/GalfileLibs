#include "../filesystem/file/file.hpp"
#include "../filesystem/folder/folder.hpp"
#include "../helpers/checksum.hpp"
#include "../dependencies/json/json.hpp"
#include <iostream>

int main()
{
    auto file = galfile::filesystem::file::open_existing("test/example/test.txt", "01.txt", true);

    std::cout << "Opened: " << file.is_opened() << std::endl;

    auto folder = galfile::filesystem::folder::create_new("dir", false);
    folder.append_file(file);

    std::cout << "File exists: " << folder.is_file_exists(file.get_name()) << std::endl;

    folder = std::move(folder);

    std::cout << "Can append folder itself: " << folder.append_folder(folder) << std::endl;

    nlohmann::json json;
    json["info"] = "success";

    std::cout << json["info"] << std::endl;

    return 0;
}