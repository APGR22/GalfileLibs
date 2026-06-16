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

    unsigned char buffer[] = "TEST_0123456789";
    file->write(0, buffer, sizeof(buffer)-1);

    std::cout
    << file->get_filepath().generic_string()
    << ": "
    << file->is_opened()
    << '\n';

    file->close();

    nlohmann::json json;
    file->save_configuration(json);

    auto load_file = galfile::filesystem::file::load_configuration(json);

    std::cout
    << load_file->get_filepath().generic_string()
    << ": "
    << load_file->is_opened()
    << '\n';

    unsigned char buffer_output[sizeof(buffer)-1];
    std::cout
    << "load_file read: "
    << load_file->read(0, buffer_output, sizeof(buffer_output))
    << '\n';

    for (uint64_t index = 0; index < sizeof(buffer_output); index++)
    {
        putc(buffer_output[index], stdout);
    }
    putc('\n', stdout);

    return 0;
}
