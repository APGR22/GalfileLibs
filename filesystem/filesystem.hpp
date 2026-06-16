#pragma once

#include <filesystem>
#include <string>
#include <memory>
#include <utility>
#include <map>
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

            void _tree_recursive__expensive(
                const std::shared_ptr<folder::Folder> &next_folder_ptr,
                const std::string &buffer_indent_begin = "",
                const std::string &indent_arrow = "-",
                const std::string &indent_spaces = " "
            ) const
            {
                std::string buffer_member_indent_begin =
                    buffer_indent_begin + "|" + indent_arrow
                ;
                std::string buffer_next_indent_begin =
                    buffer_indent_begin + "|" + indent_spaces
                ;
                std::string buffer_next_last_indent_begin =
                    buffer_indent_begin + " " + indent_spaces
                ;

                const auto &folders = next_folder_ptr->get_all_folders();
                const auto &files = next_folder_ptr->get_all_files();

                std::map<
                    std::string,
                    std::shared_ptr<folder::Folder>
                > sorted_map;

                for (const auto &folder : folders)
                {
                    sorted_map[folder.first] = folder.second;
                }

                for (const auto &file : files)
                {
                    sorted_map[file.first] = {};
                }

                std::string buffer_next;
                for (
                    auto it = sorted_map.begin();
                    it != sorted_map.end();
                    it++
                )
                {
                    const auto &pair = *it;

                    const auto &name = pair.first;
                    const auto &item = pair.second;

                    std::cout << buffer_member_indent_begin + name << std::endl;

                    auto next_it = ++it;
                    it--;
                    if (next_it == sorted_map.end())
                    {
                        buffer_next = buffer_next_last_indent_begin;
                    }
                    else
                    {
                        buffer_next = buffer_next_indent_begin;
                    }

                    if (item)
                    {
                        this->_tree_recursive__expensive(
                            item,
                            buffer_next,
                            indent_arrow,
                            indent_spaces
                        );
                    }
                }
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

            void tree__expensive(
                const path::Path &path = "/",
                uint64_t indent = 4
            ) const
            {
                auto folder_ptr = this->_go_to_folder(path);
                auto shared_folder_ptr = folder_ptr.lock();
                if (!shared_folder_ptr)
                {
                    std::cout << "(ERROR: Not found)" << std::endl;
                    return;
                }

                std::cout << shared_folder_ptr->get_name() << std::endl;

                std::string indent_arrow(indent, '-');
                std::string indent_spaces(indent, ' ');

                indent_arrow += "> ";
                indent_spaces += "  ";

                this->_tree_recursive__expensive(
                    shared_folder_ptr,
                    "",
                    indent_arrow,
                    indent_spaces
                );
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

            std::weak_ptr<folder::Folder> cpdir(
                const path::Path &src_path,
                const path::Path &dst_parent_path
            )
            {
                auto src_folder_ptr = this->_go_to_folder(src_path);
                auto dst_parent_folder_ptr = this->_go_to_folder(dst_parent_path);

                auto shared_src_folder_ptr = src_folder_ptr.lock();
                auto shared_dst_parent_folder_ptr = dst_parent_folder_ptr.lock();

                if (!shared_src_folder_ptr) return {};
                if (!shared_dst_parent_folder_ptr) return {};

                return shared_dst_parent_folder_ptr->append_folder(
                    std::make_shared<folder::Folder>(
                        *shared_src_folder_ptr
                    )
                );
            }

            std::weak_ptr<file::File> cpfile(
                const path::Path &src_filepath,
                const path::Path &dst_parent_filepath
            )
            {
                if (src_filepath == "/") return {};

                auto filename = src_filepath.name();

                auto src_parent_filepath = src_filepath.parent();

                auto src_parent_filepath_ptr = this->_go_to_folder(src_parent_filepath);
                auto dst_parent_filepath_ptr = this->_go_to_folder(dst_parent_filepath);

                auto shared_src_parent_filepath_ptr = src_parent_filepath_ptr.lock();
                auto shared_dst_parent_filepath_ptr = dst_parent_filepath_ptr.lock();

                if (!shared_src_parent_filepath_ptr) return {};
                if (!shared_dst_parent_filepath_ptr) return {};

                auto src_file_ptr = shared_src_parent_filepath_ptr->get_file(filename);
                auto shared_src_file_ptr = src_file_ptr.lock();
                if (!shared_src_file_ptr) return {};

                return shared_dst_parent_filepath_ptr->append_file(
                    std::make_shared<file::File>(
                        *shared_src_file_ptr
                    )
                );
            }

            std::weak_ptr<folder::Folder> mvdir(
                const path::Path &src_path,
                const path::Path &dst_parent_path
            )
            {
                if (src_path == "/") return {};

                auto src_parent_path = src_path.parent();

                auto src_folder_ptr = this->_go_to_folder(src_path);
                auto src_parent_folder_ptr = this->_go_to_folder(src_parent_path);
                auto dst_parent_folder_ptr = this->_go_to_folder(dst_parent_path);

                auto shared_src_folder_ptr = src_folder_ptr.lock();
                auto shared_src_parent_folder_ptr = src_parent_folder_ptr.lock();
                auto shared_dst_parent_folder_ptr = dst_parent_folder_ptr.lock();

                if (!shared_src_folder_ptr) return {};
                if (!shared_src_parent_folder_ptr) return {};
                if (!shared_dst_parent_folder_ptr) return {};

                auto mv_ret = shared_dst_parent_folder_ptr->append_folder(
                    shared_src_folder_ptr
                );
                if (mv_ret.expired()) return {};

                auto rm_ret = shared_src_parent_folder_ptr->remove_folder(
                    shared_src_folder_ptr->get_name()
                );
                if (!rm_ret) return {};

                return mv_ret;
            }

            std::weak_ptr<file::File> mvfile(
                const path::Path &src_filepath,
                const path::Path &dst_parent_filepath
            )
            {
                if (src_filepath == "/") return {};

                auto filename = src_filepath.name();

                auto src_parent_filepath = src_filepath.parent();

                auto src_parent_filepath_ptr = this->_go_to_folder(src_parent_filepath);
                auto dst_parent_filepath_ptr = this->_go_to_folder(dst_parent_filepath);

                auto shared_src_parent_filepath_ptr = src_parent_filepath_ptr.lock();
                auto shared_dst_parent_filepath_ptr = dst_parent_filepath_ptr.lock();

                if (!shared_src_parent_filepath_ptr) return {};
                if (!shared_dst_parent_filepath_ptr) return {};

                auto src_file_ptr = shared_src_parent_filepath_ptr->get_file(filename);
                auto shared_src_file_ptr = src_file_ptr.lock();
                if (!shared_src_file_ptr) return {};

                auto mv_ret = shared_dst_parent_filepath_ptr->append_file(
                    shared_src_file_ptr
                );
                if (mv_ret.expired()) return {};

                auto rm_ret = shared_src_parent_filepath_ptr->remove_file(filename);
                if (!rm_ret) return {};

                return mv_ret;
            }

            bool isdir(const path::Path &path) const
            {
                auto temp_ptr = this->_go_to_folder(path);
                return !temp_ptr.expired();
            }

            bool isfile(const path::Path &filepath) const
            {
                auto directory_path = filepath.parent();
                auto filename = filepath.name();

                auto directory_ptr = this->_go_to_folder(directory_path);
                auto shared_directory_ptr = directory_ptr.lock();
                if (!shared_directory_ptr) return false;

                return shared_directory_ptr->is_file_exists(filename);
            }

            bool isexists(const path::Path &path) const
            {
                if (this->isdir(path)) return true;
                if (this->isfile(path)) return true;

                return false;
            }
    };
}
