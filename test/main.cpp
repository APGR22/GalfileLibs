#include <memory>
#include <iostream>
#include <cstring>
#include "../io/object/multi_file.hpp"
#include "../filesystem/file/file.hpp"

int main()
{
    galfile::io::object::MultiFile multifile("test/example/test.txt", 5);
    auto file = galfile::filesystem::file::create_new_from_value(multifile, "test");

    unsigned char text[] = "1234567890";
    file->write(0, text, sizeof(text)-1);

    file->write_c(1, '1');

    std::cout << (char)(file->read_c(2)) << '\n';

    unsigned char get_text[sizeof(text) + 1];
    memset(get_text, 0, sizeof(get_text));

    file->read(0, get_text, sizeof(get_text) - 1);

    std::cout << reinterpret_cast<char *>(get_text) << '\n';

    multifile.fclose();

    return 0;
}
