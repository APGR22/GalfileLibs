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
            std::unique_ptr<folder::Folder> root;
            folder::Folder *__curdir_ptr;

        public:
            Filesystem()
            :
                root(std::make_unique<folder::Folder>(folder::create_new("/"))),
                __curdir_ptr(this->root.get())
            {}

            folder::Folder *cd(const path::Path &path)
            {
                if (path.is_absolute())
                {
                    this->__curdir_ptr = this->root.get();
                }

                const auto &path_parts = path.parts();

                folder::Folder *temp_ptr = nullptr;
                // skip first element ["." or "/"]
                for (auto it = ++path_parts.begin(); it != path_parts.end(); it++)
                {
                    const std::string &pathname = *it;

                    temp_ptr = this->__curdir_ptr->get_folder(pathname);
                    if (!temp_ptr) return nullptr;

                    this->__curdir_ptr = temp_ptr;
                }

                return this->__curdir_ptr;
            }

            folder::Folder *mkdirs(const path::Path &path)
            {
                folder::Folder *curdir_ptr = nullptr;
                if (path.is_absolute())
                {
                    curdir_ptr = this->root.get();
                }
                else
                {
                    curdir_ptr = this->__curdir_ptr;
                }

                const auto &path_parts = path.parts();
                // skip first element ["." or "/"]
                for (auto it = ++path_parts.begin(); it != path_parts.end(); it++)
                {
                    const std::string &pathname = *it;

                    curdir_ptr = curdir_ptr->append_folder(
                        folder::create_new(pathname, true, curdir_ptr)
                    );
                    if (curdir_ptr == nullptr) return nullptr;
                }

                return curdir_ptr;
            }
    };
}