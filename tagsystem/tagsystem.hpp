#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <list>
#include <algorithm>
#include "tag/object/object.hpp"

namespace galfile::tagsystem
{
    class TagSystem
    {
        private:
            std::unordered_map<
                std::string,
                std::list<
                    std::shared_ptr<tag::object::Object>
                >
            > objects;

        public:
            bool add_tag(const std::string &tag_name)
            {
                if (this->has_tag(tag_name)) return false;

                this->objects[tag_name];

                return true;
            }

            bool add_object_at_tag(
                const std::string &tag_name,
                const std::shared_ptr<tag::object::Object> &object
            )
            {
                auto &list_objects = this->objects[tag_name];
                if (
                    std::find(
                        list_objects.begin(),
                        list_objects.end(),
                        object
                    )
                    != list_objects.end()
                )
                {
                    return false;
                }

                list_objects.push_back(object);
                object->add_tag(tag_name);

                return true;
            }

            auto get_tags() const
            {
                std::list<std::string> list_tags;

                for (const auto &pair : this->objects)
                {
                    list_tags.push_back(pair.first);
                }

                return list_tags;
            }

            bool get_objects_at_tag(
                const std::string &tag_name,
                std::list<std::shared_ptr<tag::object::Object>> &dst_list
            ) const
            {
                if (!this->has_tag(tag_name)) return false;

                dst_list = this->objects.at(tag_name);

                return true;
            }

            bool remove_tag(const std::string &tag_name)
            {
                auto it = this->objects.find(tag_name);
                if (it == this->objects.end()) return false;

                auto &list_objects = it->second;

                for (auto &__object : list_objects)
                {
                    __object->remove_tag(tag_name);
                }

                this->objects.erase(it);
                return true;
            }

            bool remove_object_at_tag(
                const std::string &tag_name,
                const std::shared_ptr<tag::object::Object> &object
            )
            {
                auto tag_it = this->objects.find(tag_name);
                if (tag_it == this->objects.end()) return false;

                auto &list_objects = tag_it->second;
                auto object_it = std::find(
                    list_objects.begin(),
                    list_objects.end(),
                    object
                );
                if (object_it == list_objects.end()) return false;

                auto __object = object_it->get();

                __object->remove_tag(tag_name);
                list_objects.erase(object_it);

                return true;
            }

            bool has_tag(const std::string &tag_name) const
            {
                return this->objects.contains(tag_name);
            }

            bool has_object_at_tag(
                const std::string &tag_name,
                const std::shared_ptr<tag::object::Object> &object
            ) const
            {
                auto tag_it = this->objects.find(tag_name);
                if (tag_it == this->objects.end()) return false;

                auto &list_objects = tag_it->second;

                return std::find(
                    list_objects.begin(),
                    list_objects.end(),
                    object
                )
                != list_objects.end();
            }
    };
}