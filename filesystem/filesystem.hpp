#pragma once

#include <filesystem>
#include <string>
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
            path::Path __curdir_path;

        protected:
            std::weak_ptr<folder::Folder> _go_to_folder(
                const path::Path &path
            ) const
            {
                std::weak_ptr<folder::Folder> curdir_ptr;
                if (path.is_absolute())
                {
                    curdir_ptr = this->__root;
                }
                else
                {
                    curdir_ptr = this->__curdir_ptr;
                }

                const auto &path_parts = path.parts();
                auto shared_curdir_ptr = curdir_ptr.lock();
                std::weak_ptr<folder::Folder> temp_curdir_ptr;
                // skip first element ["." or "/"]
                for (
                    auto it = ++path_parts.begin();
                    it != path_parts.end();
                    it++
                )
                {
                    const std::string &path_name = *it;

                    if (!shared_curdir_ptr) return {};

                    temp_curdir_ptr = shared_curdir_ptr->get_folder(path_name);
                    if (temp_curdir_ptr.expired()) return {};

                    curdir_ptr = temp_curdir_ptr;
                    shared_curdir_ptr = curdir_ptr.lock();
                }

                return curdir_ptr;
            }

        public:
            Filesystem()
            :
                __root(folder::create_new("/")),
                __curdir_ptr(this->__root),
                __curdir_path("/")
            {}

            std::weak_ptr<folder::Folder> cd(const path::Path &path)
            {
                auto temp_curdir_ptr = this->_go_to_folder(path);
                if (temp_curdir_ptr.expired()) return {};

                this->__curdir_ptr = temp_curdir_ptr;

                this->__curdir_path /= path;

                return this->__curdir_ptr;
            }

            std::string pwd() const
            {
                return this->__curdir_path.general_string();
            }

            std::weak_ptr<folder::Folder> mkdirs(const path::Path &path)
            {
                std::weak_ptr<folder::Folder> curdir_ptr;
                if (path.is_absolute())
                {
                    curdir_ptr = this->__root;
                }
                else
                {
                    curdir_ptr = this->__curdir_ptr;
                }
                auto prevdir_ptr = curdir_ptr;

                const auto &path_parts = path.parts();
                auto shared_curdir_ptr = curdir_ptr.lock();
                // skip first element ["." or "/"]
                for (
                    auto it = ++path_parts.begin();
                    it != path_parts.end();
                    it++
                )
                {
                    if (!shared_curdir_ptr) return {};

                    const std::string &pathname = *it;

                    curdir_ptr = shared_curdir_ptr->append_folder(
                        folder::create_new(pathname, true, curdir_ptr)
                    );

                    shared_curdir_ptr = curdir_ptr.lock();
                    if (!shared_curdir_ptr) return {};

                    shared_curdir_ptr->set_parent(prevdir_ptr);
                    prevdir_ptr = curdir_ptr;
                }

                return curdir_ptr;
            }

            std::weak_ptr<file::File> mkfile(
                const path::Path &filepath,
                const std::filesystem::path &physical_filepath
            )
            {
                auto directory_path = filepath.parent();
                auto filename = filepath.name();

                auto folder_ptr = this->_go_to_folder(directory_path);
                auto shared_folder_ptr = folder_ptr.lock();
                if (!shared_folder_ptr) return {};

                if (shared_folder_ptr->is_file_exists(filename)) return {};

                return shared_folder_ptr->append_file(
                    file::create_new(
                        physical_filepath,
                        filename,
                        true
                    )
                );
            }

            bool rmdirs(const path::Path &path)
            {
                auto folder_ptr = this->_go_to_folder(path);
                auto shared_folder_ptr = folder_ptr.lock();
                if (!shared_folder_ptr) return false;

                if (shared_folder_ptr.get() == this->__root.get())
                {
                    this->__root->clear();

                    this->__curdir_ptr = this->__root;
                    this->__curdir_path = "/";

                    return true;
                }

                auto parent_folder_ptr = shared_folder_ptr->get_parent();
                auto shared_parent_folder_ptr = parent_folder_ptr.lock();

                this->__curdir_ptr = parent_folder_ptr;
                this->__curdir_path = path.parent();

                return shared_parent_folder_ptr->remove_folder(
                    shared_folder_ptr->get_name()
                );
            }

            bool rmfile(const path::Path &filepath)
            {
                auto directory_path = filepath.parent();
                auto filename = filepath.name();

                auto folder_ptr = this->_go_to_folder(directory_path);
                auto shared_folder_ptr = folder_ptr.lock();
                if (!shared_folder_ptr) return false;

                if (!shared_folder_ptr->is_file_exists(filename)) return false;

                return shared_folder_ptr->remove_file(filename);
            }

            bool isdir(const path::Path &path) const
            {
                auto temp_ptr = this->_go_to_folder(path);
                return !temp_ptr.expired();
            }
    };
}
