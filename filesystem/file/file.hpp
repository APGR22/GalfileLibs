#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "../../type/int.hpp"

namespace galfile::filesystem::file
{
    class File
    {
        private:
            FILE *file_ptr = nullptr;
            std::string name;
            std::filesystem::path filepath;
            bool auto_close = true;

        public:
            File(FILE *file_ptr, const std::string &name, const std::filesystem::path &filepath, bool auto_close)
            : file_ptr(file_ptr), name(name), filepath(filepath), auto_close(auto_close)
            {}

            File(const File &other)
            : file_ptr(other.file_ptr), name(other.name), filepath(other.filepath), auto_close(other.auto_close)
            {}

            File(File &&other)
            : file_ptr(other.file_ptr), name(other.name), filepath(other.filepath), auto_close(other.auto_close)
            {
                if (this == &other) return;

                other.file_ptr = nullptr;
                other.name.clear();
                other.filepath.clear();
            }

            int read_c(type::seekpos_t seekpos) const
            {
                if (this->file_ptr == nullptr) return -1;

                fseek(this->file_ptr, seekpos, SEEK_SET);
                return fgetc(this->file_ptr);
            }

            size_t read(type::seekpos_t seekpos, unsigned char *dst_bytes, size_t max_size) const
            {
                if (this->file_ptr == nullptr) return 0;
                if (dst_bytes == nullptr) return 0;
                if (max_size == 0) return 0;

                fseek(this->file_ptr, seekpos, SEEK_SET);

                return fread(dst_bytes, sizeof(unsigned char), max_size, this->file_ptr);
            }

            type::sizec_t write_c(type::seekpos_t seekpos, unsigned char byte)
            {
                if (this->file_ptr == nullptr) return 0;

                fseek(this->file_ptr, seekpos, SEEK_SET);
                int ret = fputc(byte, this->file_ptr);

                if (ret != EOF) return 1;
                else return 0;
            }

            size_t write_set(type::seekpos_t seekpos, unsigned char byte, size_t size)
            {
                if (this->file_ptr == nullptr) return 0;

                fseek(this->file_ptr, seekpos, SEEK_SET);

                unsigned char buffer[size];
                memset(buffer, byte, size);

                return fwrite(buffer, sizeof(unsigned char), size, this->file_ptr);
            }

            size_t write(type::seekpos_t seekpos, const unsigned char *bytes, size_t size)
            {
                if (this->file_ptr == nullptr) return 0;
                if (bytes == nullptr) return 0;
                if (size == 0) return 0;

                fseek(this->file_ptr, seekpos, SEEK_SET);
                return fwrite(bytes, sizeof(unsigned char), size, this->file_ptr);
            }

            bool clear()
            {
                this->close();

                std::string filepath_s = this->filepath.generic_string();
                FILE *temp = fopen(filepath_s.c_str(), "w");
                if (temp)
                {
                    fclose(temp);
                    temp = nullptr;
                }
                else
                {
                    return false;
                }

                this->file_ptr = fopen(filepath_s.c_str(), "rb+");

                return true;
            }

            void close()
            {
                if (this->file_ptr && this->auto_close)
                {
                    fclose(this->file_ptr);
                    this->file_ptr = nullptr;
                }
            }

            void set_auto_close(bool value)
            {
                this->auto_close = value;
            }

            size_t get_size() const
            {
                if (this->file_ptr == nullptr) return 0;

                fseek(this->file_ptr, 0, SEEK_END);
                return static_cast<size_t>( ftell(this->file_ptr) );
            }

            const std::string &get_name() const
            {
                return this->name;
            }

            const std::filesystem::path &get_filepath() const
            {
                return this->filepath;
            }

            bool is_opened() const
            {
                return this->file_ptr != nullptr;
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

                    this->file_ptr = other.file_ptr;
                    this->name = other.name;
                    this->filepath = other.filepath;
                    this->auto_close = other.auto_close;
                }

                return *this;
            }

            File &operator=(File &&other)
            {
                if (this != &other)
                {
                    this->close();

                    this->file_ptr = other.file_ptr;
                    this->name = other.name;
                    this->filepath = other.filepath;
                    this->auto_close = other.auto_close;

                    other.file_ptr = nullptr;
                    other.filepath.clear();
                    other.name.clear();
                }

                return *this;
            }

            bool operator==(const File &other) const
            {
                return this->file_ptr == other.file_ptr;
            }

            ~File()
            {
                this->close();
            }
    };

    inline File open_existing(const std::filesystem::path &filepath, const std::string &name, bool auto_close = true)
    {
        std::string filepath_s = filepath.generic_string();
        FILE *file_ptr = fopen(filepath_s.c_str(), "rb+");

        return File(file_ptr, name, filepath, auto_close);
    }

    inline File create_new(const std::filesystem::path &filepath, const std::string &name, bool auto_close = true)
    {
        std::string filepath_s = filepath.generic_string();
        FILE *temp = fopen(filepath_s.c_str(), "w");
        if (temp)
        {
            temp == nullptr;
        }

        return open_existing(filepath, name, auto_close);
    }
}