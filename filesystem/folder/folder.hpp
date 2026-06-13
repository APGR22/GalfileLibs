#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "folder_fwd.hpp"
#include "../file/file.hpp"

namespace galfile::filesystem::folder
{
    class Folder : protected std::enable_shared_from_this<Folder>
    {
        private:
            std::weak_ptr<Folder> __parent;
            std::string __name;
            std::unordered_map<std::string, std::shared_ptr<file::File>> __files;
            std::unordered_map<std::string, std::shared_ptr<Folder>> __folders;

            bool __auto_close_files = false;

        public:
            Folder(
                std::weak_ptr<Folder> parent,
                const std::string &name,
                bool auto_close_files
            )
            :
                __parent(parent),
                __name(name),
                __auto_close_files(auto_close_files)
            {}

            Folder(const Folder &other)
            :
                __parent(other.__parent),
                __name(other.__name),
                __files(other.__files),
                __folders(other.__folders),
                __auto_close_files(other.__auto_close_files)
            {}

            Folder(Folder &&other)
            :
                __parent(other.__parent),
                __name(other.__name),
                __files(std::move(other.__files)),
                __folders(std::move(other.__folders)),
                __auto_close_files(other.__auto_close_files)
            {
                other.__parent.reset();
                other.__name.clear();
                other.__files.clear();
                other.__folders.clear();
            }

            std::weak_ptr<file::File> append_file(const std::shared_ptr<file::File> &file)
            {
                if (this->__files.contains(file->get_name())) return {};
                if (this->__folders.contains(file->get_name())) return {};

                auto pair = this->__files.insert({file->get_name(), file});
                auto &item = pair.first->second;

                return item;
            }

            std::weak_ptr<Folder> append_folder(const std::shared_ptr<Folder> &folder)
            {
                if (this == folder.get()) return {};
                if (this->__folders.contains(folder->__name)) return {};
                if (this->__files.contains(folder->__name)) return {};

                auto pair = this->__folders.insert({folder->__name, folder});
                auto &item = pair.first->second;
                item->__parent = this->weak_from_this();

                return item;
            }

            void set_parent(const std::weak_ptr<Folder> &parent)
            {
                this->__parent = parent;
            }

            const auto &get_parent() const
            {
                return this->__parent;
            }

            const std::string &get_name() const
            {
                return this->__name;
            }

            std::weak_ptr<file::File> get_file(const std::string &name)
            {
                if (!this->__files.contains(name))
                {
                    return {};
                }

                return this->__files.at(name);
            }

            std::weak_ptr<Folder> get_folder(const std::string &name)
            {
                if (!this->__folders.contains(name))
                {
                    return {};
                }

                return this->__folders.at(name);
            }

            const auto &get_all_files() const
            {
                return this->__files;
            }

            const auto &get_all_folders() const
            {
                return this->__folders;
            }

            bool remove_file(const std::string &name)
            {
                if (!this->__files.contains(name)) return false;

                this->__files.at(name)->set_auto_close(this->__auto_close_files);
                this->__files.erase(name);

                return true;
            }

            bool remove_folder(const std::string &name)
            {
                if (!this->__folders.contains(name)) return false;

                this->__folders.at(name)->__parent.reset();
                this->__folders.erase(name);

                return true;
            }

            void clear()
            {
                for (auto &it : this->__files)
                {
                    auto &file = it.second;
                    file->set_auto_close(this->__auto_close_files);
                }

                this->__files.clear();
                this->__folders.clear();
            }

            void set_auto_close_files(bool value)
            {
                this->__auto_close_files = value;
            }

            bool is_file_exists(const std::string &name) const
            {
                return this->__files.contains(name);
            }

            bool is_folder_exists(const std::string &name) const
            {
                return this->__folders.contains(name);
            }

            bool is_empty() const noexcept
            {
                return this->__files.empty() && this->__folders.empty();
            }

            Folder &operator=(const Folder &other)
            {
                if (this != &other)
                {
                    this->__parent = other.__parent;
                    this->__name = other.__name;
                    this->__files = other.__files;
                    this->__folders = other.__folders;
                    this->__auto_close_files = other.__auto_close_files;
                }

                return *this;
            }

            Folder &operator=(Folder &&other)
            {
                if (this != &other)
                {
                    this->__parent = other.__parent;
                    this->__name = other.__name;
                    this->__files = std::move(other.__files);
                    this->__folders = std::move(other.__folders);
                    this->__auto_close_files = other.__auto_close_files;

                    other.__parent.reset();
                    other.__name.clear();
                    other.__files.clear();
                    other.__folders.clear();
                }

                return *this;
            }

            ~Folder()
            {
                this->__parent.reset();
                this->__name.clear();
                this->__files.clear();
                this->__folders.clear();

                this->clear();
            }
    };

    inline auto create_new(
        const std::string &name,
        bool auto_close_files = false,
        std::weak_ptr<Folder> parent = {}
    )
    {
        return std::make_shared<Folder>(Folder(
            parent,
            name,
            auto_close_files
        ));
    }
}