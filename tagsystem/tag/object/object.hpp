#pragma once

#include <list>
#include <algorithm>
#include <string>
#include <memory>
#include "../../../filesystem/file/file.hpp"

namespace galfile::tagsystem::tag::object
{
    class Object
    {
        private:
            std::shared_ptr<filesystem::file::File> file;
            std::list<std::string> tags;

        public:
            Object(const std::shared_ptr<filesystem::file::File> &file)
            : file(file)
            {}

            bool add_tag(const std::string &tag_name)
            {
                if (this->has_tag(tag_name)) return false;

                this->tags.push_back(tag_name);

                return true;
            }

            const auto &get() const
            {
                return this->file;
            }

            const auto &get_tags() const
            {
                return this->tags;
            }

            bool remove_tag(const std::string &tag_name)
            {
                auto it = std::find(
                    this->tags.begin(),
                    this->tags.end(),
                    tag_name
                );
                if (it == this->tags.end()) return false;

                this->tags.erase(it);

                return true;
            }

            bool empty_tags()
            {
                return this->tags.empty();
            }

            bool has_tag(const std::string &tag_name) const
            {
                return std::find(
                    this->tags.begin(),
                    this->tags.end(),
                    tag_name
                )
                != this->tags.end();
            }

            bool is_tags_empty() const
            {
                return this->tags.empty();
            }

            auto *operator->() const
            {
                return this->file.get();
            }

            bool operator==(const filesystem::file::File &other_file) const
            {
                return (*this->file) == other_file;
            }

            bool operator==(const Object &other) const
            {
                return (*this->file) == (*other.file);
            }
    };
}