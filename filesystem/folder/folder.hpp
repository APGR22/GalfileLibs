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
            std::weak_ptr<Folder> parent;
            std::string name;
            std::unordered_map<std::string, std::shared_ptr<file::File>> files;
            std::unordered_map<std::string, std::shared_ptr<Folder>> folders;

            bool auto_close_files = false;

        public:
            Folder(
                std::weak_ptr<Folder> parent,
                const std::string &name,
                bool auto_close_files
            )
            :
                parent(parent),
                name(name),
                auto_close_files(auto_close_files)
            {}

            Folder(const Folder &other)
            :
                parent(other.parent),
                name(other.name),
                files(other.files),
                folders(other.folders),
                auto_close_files(other.auto_close_files)
            {}

            Folder(Folder &&other)
            :
                parent(other.parent),
                name(other.name),
                files(std::move(other.files)),
                folders(std::move(other.folders)),
                auto_close_files(other.auto_close_files)
            {
                other.parent.reset();
                other.name.clear();
                other.files.clear();
                other.folders.clear();
            }

            std::weak_ptr<file::File> append_file(const std::shared_ptr<file::File> &file)
            {
                if (this->files.contains(file->get_name())) return {};
                if (this->folders.contains(file->get_name())) return {};

                auto pair = this->files.insert({file->get_name(), file});
                auto &item = pair.first->second;

                return item;
            }

            std::weak_ptr<Folder> append_folder(const std::shared_ptr<Folder> &folder)
            {
                if (this == folder.get()) return {};
                if (this->folders.contains(folder->name)) return {};
                if (this->files.contains(folder->name)) return {};

                auto pair = this->folders.insert({folder->name, folder});
                auto &item = pair.first->second;
                item->parent = this->weak_from_this();

                return item;
            }

            const std::string &get_name() const
            {
                return this->name;
            }

            std::weak_ptr<file::File> get_file(const std::string &name)
            {
                if (!this->files.contains(name))
                {
                    return {};
                }

                return this->files.at(name);
            }

            std::weak_ptr<Folder> get_folder(const std::string &name)
            {
                if (!this->folders.contains(name))
                {
                    return {};
                }

                return this->folders.at(name);
            }

            const auto &get_all_files() const
            {
                return this->files;
            }

            const auto &get_all_folders() const
            {
                return this->folders;
            }

            bool remove_file(const std::string &name)
            {
                if (!this->files.contains(name)) return false;

                this->files.at(name)->set_auto_close(this->auto_close_files);
                this->files.erase(name);

                return true;
            }

            bool remove_folder(const std::string &name)
            {
                if (!this->folders.contains(name)) return false;

                this->folders.at(name)->parent.reset();
                this->folders.erase(name);

                return true;
            }

            void clear()
            {
                for (auto &it : this->files)
                {
                    auto &file = it.second;
                    file->set_auto_close(this->auto_close_files);
                }

                this->files.clear();
                this->folders.clear();
            }

            void set_auto_close_files(bool value)
            {
                this->auto_close_files = value;
            }

            bool is_file_exists(const std::string &name) const
            {
                return this->files.contains(name);
            }

            bool is_folder_exists(const std::string &name) const
            {
                return this->folders.contains(name);
            }

            bool is_empty() const noexcept
            {
                return this->files.empty() && this->folders.empty();
            }

            Folder &operator=(const Folder &other)
            {
                if (this != &other)
                {
                    this->parent = other.parent;
                    this->name = other.name;
                    this->files = other.files;
                    this->folders = other.folders;
                    this->auto_close_files = other.auto_close_files;
                }

                return *this;
            }

            Folder &operator=(Folder &&other)
            {
                if (this != &other)
                {
                    this->parent = other.parent;
                    this->name = other.name;
                    this->files = std::move(other.files);
                    this->folders = std::move(other.folders);
                    this->auto_close_files = other.auto_close_files;

                    other.parent.reset();
                    other.name.clear();
                    other.files.clear();
                    other.folders.clear();
                }

                return *this;
            }

            ~Folder()
            {
                this->parent.reset();
                this->name.clear();

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