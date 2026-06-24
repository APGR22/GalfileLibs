#include <memory>
#include <iostream>
#include <cstring>
#include "../io/object/multi_file.hpp"
#include "../filesystem/file/file.hpp"
#include "../filesystem/filesystem.hpp"

int main()
{
    galfile::io::object::MultiFile multifile("test/example/test.txt", 5);
    auto file = galfile::filesystem::file::create_new_from_value(multifile, "test");

    galfile::filesystem::Filesystem filesystem;

    auto folder = filesystem.mkdirs("/media/linux");
    folder->append_file(file);
    filesystem.mkfile("/media/linux/test_1.txt", "test/example/test_1.txt");

    folder = filesystem.mkdirs("/media/windows");
    filesystem.mkfile("/media/windows/test", "test/example/test_2.txt");

    nlohmann::json json;

    filesystem.save_configuration(json);

    galfile::io::object::SingleFile configuration_file("test/example/filesystem.json");
    configuration_file.fopen(galfile::io::NEW_EMPTY_AND_WRITE);

    std::string JSON = json.dump(4);
    configuration_file.fwrite(JSON.c_str(), sizeof(char), JSON.size());

    configuration_file.fclose();

    return 0;
}
