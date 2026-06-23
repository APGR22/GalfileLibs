#include <memory>
#include <iostream>
#include <cstring>
#include "../io/object/multi_file.hpp"

int main()
{
    galfile::io::object::MultiFile multifile("test/example/test.txt", 5);

    multifile.fopen(galfile::io::IOMode::NEW_EMPTY_AND_WRITE);
    multifile.fclose();
    multifile.fopen(galfile::io::IOMode::KEEP_EXISTING_AND_READ_WRITE);

    unsigned char text[] = "1234567890";
    multifile.fwrite(text, sizeof(unsigned char), sizeof(text)-1);

    multifile.fputc('1');

    multifile.fseek(-1, SEEK_CUR);
    std::cout << (char)(multifile.fgetc()) << '\n';

    unsigned char get_text[sizeof(text) + 1];
    memset(get_text, 0, sizeof(get_text));

    multifile.fseek(0, SEEK_SET);
    multifile.fread(get_text, sizeof(unsigned char), sizeof(get_text) - 1);

    std::cout << reinterpret_cast<char *>(get_text) << '\n';

    multifile.fclose();

    return 0;
}
