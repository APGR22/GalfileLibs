#include "../filesystem/path/path.hpp"
#include <iostream>

int main()
{
    galfile::filesystem::path::Path path = "media/usb/././......./..././/.//././";

    std::cout << "Is path absolute: " << path.is_absolute() << std::endl;

    path.to_parent();
    path.to_parent();
    path.to_parent();

    path /= "linux/media/media/media./..";

    for (const auto &item : path.join_path("gamer/cool/..").parts())
    {
        std::cout << item << std::endl;
    }

    return 0;
}