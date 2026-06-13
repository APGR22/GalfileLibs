#pragma once

#include <filesystem>
#include <memory>
#include "folder/folder.hpp"
#include "path/path.hpp"

namespace galfile::filesystem
{
    class Filesystem
    {
        private:
            std::shared_ptr<folder::Folder> __root;
            std::weak_ptr<folder::Folder> __curdir_ptr;

        public:
            Filesystem()
            :
                __root(folder::create_new("/")),
                __curdir_ptr(this->__root)
            {}

            std::weak_ptr<folder::Folder> cd(const path::Path &path)
            {
                if (path.is_absolute())
                {
                    this->__curdir_ptr = this->__root;
                }

                const auto &path_parts = path.parts();

                std::weak_ptr<folder::Folder> temp_ptr;
                auto __shared_curdir_ptr = this->__curdir_ptr.lock();
                // skip first element ["." or "/"]
                for (auto it = ++path_parts.begin(); it != path_parts.end(); it++)
                {
                    const std::string &pathname = *it;

                    temp_ptr = __shared_curdir_ptr->get_folder(pathname);
                    if (temp_ptr.expired()) return {};

                    this->__curdir_ptr = temp_ptr;
                }

                return this->__curdir_ptr;
            }

            std::weak_ptr<folder::Folder> mkdirs(const path::Path &path)
            {
                auto __shared_curdir_ptr = this->__curdir_ptr.lock();
                std::weak_ptr<folder::Folder> curdir_ptr;
                if (path.is_absolute())
                {
                    curdir_ptr = this->__root;
                }
                else
                {
                    curdir_ptr = __shared_curdir_ptr;
                }
                auto prevdir_ptr = curdir_ptr;

                const auto &path_parts = path.parts();
                // skip first element ["." or "/"]
                for (auto it = ++path_parts.begin(); it != path_parts.end(); it++)
                {
                    const std::string &pathname = *it;

                    auto shared_curdir_ptr = curdir_ptr.lock();

                    curdir_ptr = shared_curdir_ptr->append_folder(
                        folder::create_new(pathname, true, curdir_ptr)
                    );
                    if (curdir_ptr.expired()) return {};

                    shared_curdir_ptr = curdir_ptr.lock();

                    shared_curdir_ptr->set_parent(prevdir_ptr);
                    prevdir_ptr = curdir_ptr;
                }

                return curdir_ptr;
            }
    };
}