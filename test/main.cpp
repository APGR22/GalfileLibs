#include "../filesystem/filesystem.hpp"
#include "../tagsystem/tagsystem.hpp"
#include <memory>
#include <iostream>

int main()
{
    auto file = galfile::filesystem::file::create_new("test/example/test.txt", "custom");
    auto tag_object = std::make_shared<galfile::tagsystem::tag::object::Object>(&file);
    galfile::tagsystem::TagSystem tagsystem;

    tagsystem.add_object_at_tag("testing", tag_object);

    std::cout << (*tag_object)->get_name() << std::endl;
    std::cout
    << "Is object present in tag \"testing\": "
    << tagsystem.has_object_at_tag("testing", tag_object)
    << std::endl
    ;
    std::cout
    << "Is tag saved to the object: "
    << tag_object->has_tag("testing")
    << std::endl
    ;

    return 0;
}