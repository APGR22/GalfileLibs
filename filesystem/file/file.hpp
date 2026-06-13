#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <type_traits>
#include <memory>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "../../io/object.hpp"
#include "../../io/object/single_file.hpp"
#include "../../type/int.hpp"
#include "../folder/folder_fwd.hpp"

namespace galfile::filesystem::file
{
    class File
    {
        private:
            std::weak_ptr<folder::Folder> __parent;
            std::shared_ptr<io::Object> __io_ptr;
            std::string __name;
            std::filesystem::path __filepath;
            bool __auto_close = true;

        public:
            File(
                std::weak_ptr<folder::Folder> parent,
                std::shared_ptr<io::Object> &io_ptr,
                const std::string &name,
                const std::filesystem::path &filepath,
                bool auto_close
            )
            :
                __parent(parent),
                __io_ptr(io_ptr),
                __name(name),
                __filepath(filepath),
                __auto_close(auto_close)
            {}

            File(const File &other)
            : __io_ptr(other.__io_ptr), __name(other.__name), __filepath(other.__filepath), __auto_close(other.__auto_close)
            {}

            File(File &&other)
            : __io_ptr(other.__io_ptr), __name(other.__name), __filepath(other.__filepath), __auto_close(other.__auto_close)
            {
                if (this == &other) return;

                other.__name.clear();
                other.__parent.reset();
                other.__filepath.clear();
            }

            int read_c(type::seekpos_t seekpos) const
            {
                if (!this->__io_ptr->is_opened()) return -1;

                this->__io_ptr->fseek(seekpos, SEEK_SET);
                return this->__io_ptr->fgetc();
            }

            size_t read(type::seekpos_t seekpos, unsigned char *dst_bytes, size_t max_size) const
            {
                if (!this->__io_ptr->is_opened()) return 0;
                if (dst_bytes == nullptr) return 0;
                if (max_size == 0) return 0;

                this->__io_ptr->fseek(seekpos, SEEK_SET);

                return this->__io_ptr->fread(dst_bytes, sizeof(unsigned char), max_size);
            }

            type::sizec_t write_c(type::seekpos_t seekpos, unsigned char byte)
            {
                if (!this->__io_ptr->is_opened()) return 0;

                this->__io_ptr->fseek(seekpos, SEEK_SET);
                int ret = this->__io_ptr->fputc(byte);

                if (ret != EOF) return 1;
                else return 0;
            }

            size_t write_set(type::seekpos_t seekpos, unsigned char byte, size_t size)
            {
                if (!this->__io_ptr->is_opened()) return 0;

                this->__io_ptr->fseek(seekpos, SEEK_SET);

                unsigned char buffer[size];
                memset(buffer, byte, size);

                return this->__io_ptr->fwrite(buffer, sizeof(unsigned char), size);
            }

            size_t write(type::seekpos_t seekpos, const unsigned char *bytes, size_t size)
            {
                if (!this->__io_ptr->is_opened()) return 0;
                if (bytes == nullptr) return 0;
                if (size == 0) return 0;

                this->__io_ptr->fseek(seekpos, SEEK_SET);
                return this->__io_ptr->fwrite(bytes, sizeof(unsigned char), size);
            }

            bool clear()
            {
                this->close();

                this->__io_ptr->fopen(io::IOMode::NEW_EMPTY_AND_WRITE);
                if (this->__io_ptr->is_opened())
                {
                    this->__io_ptr->fclose();
                }
                else
                {
                    return false;
                }

                this->__io_ptr->fopen(io::IOMode::KEEP_EXISTING_AND_READ_WRITE);

                return true;
            }

            void close()
            {
                if (this->__io_ptr->is_opened() && this->__auto_close)
                {
                    this->__io_ptr->fclose();
                }
            }

            void set_auto_close(bool value)
            {
                this->__auto_close = value;
            }

            void set_parent(const std::weak_ptr<folder::Folder> &parent)
            {
                this->__parent = parent;
            }

            size_t get_size() const
            {
                if (!this->__io_ptr->is_opened()) return 0;

                this->__io_ptr->fseek(0, SEEK_END);
                return static_cast<size_t>( this->__io_ptr->ftell() );
            }

            const std::string &get_name() const
            {
                return this->__name;
            }

            const std::filesystem::path &get_filepath() const
            {
                return this->__filepath;
            }

            const std::weak_ptr<folder::Folder> &get_parent() const
            {
                return this->__parent;
            }

            bool is_opened() const
            {
                return this->__io_ptr->is_opened();
            }

            bool is_empty() const
            {
                return this->get_size() == 0;
            }

            File &operator=(const File &other)
            {
                if (this != &other)
                {
                    this->close();

                    this->__io_ptr = other.__io_ptr;
                    this->__name = other.__name;
                    this->__filepath = other.__filepath;
                    this->__auto_close = other.__auto_close;
                }

                return *this;
            }

            File &operator=(File &&other)
            {
                if (this != &other)
                {
                    this->close();

                    this->__io_ptr = other.__io_ptr;
                    this->__name = other.__name;
                    this->__filepath = other.__filepath;
                    this->__auto_close = other.__auto_close;

                    other.__filepath.clear();
                    other.__name.clear();
                }

                return *this;
            }

            bool operator==(const File &other) const
            {
                return &(this->__io_ptr) == &(other.__io_ptr);
            }

            ~File()
            {
                this->close();
                this->__name.clear();
                this->__parent.reset();
                this->__filepath.clear();
            }
    };

    template<class T = io::object::SingleFile>
    std::enable_if_t<
        std::is_base_of_v<io::Object, T>,
        std::shared_ptr<File>
    >
        open_existing(const std::filesystem::path &filepath, const std::string &name, bool auto_close = true)
    {
        std::shared_ptr<io::Object> io_object = std::make_shared<T>(T(filepath));
        io_object->fopen(io::IOMode::KEEP_EXISTING_AND_READ_WRITE);

        return std::make_shared<File>(File({}, io_object, name, filepath, auto_close));
    }

    template<class T = io::object::SingleFile>
    std::enable_if_t<
        std::is_base_of_v<io::Object, T>,
        std::shared_ptr<File>
    >
        create_new(const std::filesystem::path &filepath, const std::string &name, bool auto_close = true)
    {
        std::shared_ptr<io::Object> io_object = std::make_shared<T>(T(filepath));
        io_object->fopen(io::IOMode::NEW_EMPTY_AND_WRITE);
        if (io_object->is_opened())
        {
            io_object->fclose();
        }

        io_object->fopen(io::IOMode::KEEP_EXISTING_AND_READ_WRITE);

        return std::make_shared<File>(File({}, io_object, name, filepath, auto_close));
    }
}

#include "../folder/folder.hpp"