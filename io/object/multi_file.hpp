#pragma once

#include <filesystem>
#include <vector>
#include <cstdio>
#include "../object.hpp"

namespace galfile::io::object
{
    class MultiFile : public io::Object
    {
        private:
            std::vector<FILE *> __files;
            size_t __max_size_per_file;

            size_t __data_size = 0;
            uint64_t __cur_index = 0;

        protected:
            FILE *_create_new(uint64_t file_index)
            {
                std::string filename =
                    this->_filepath.generic_string()
                    + "_"
                    + std::to_string(file_index)
                ;

                FILE *file = nullptr;

                // Create new
                file = ::fopen(filename.c_str(), "w");
                if (!file) return nullptr;

                ::fclose(file);

                // Open
                file = ::fopen(filename.c_str(), "rb+");
                if (!file) return nullptr;

                return file;
            }

            FILE *_try_to_get_file(uint64_t file_index)
            {
                if (file_index >= this->__files.size()) return nullptr;

                std::string filename =
                    this->_filepath.generic_string()
                    + "_"
                    + std::to_string(file_index)
                ;

                FILE *&file = this->__files[file_index];
                if (!file)
                {
                    file = ::fopen(filename.c_str(), "rb+");
                    if (!file)
                    {
                        if (std::filesystem::is_regular_file(filename)) return nullptr;

                        file = ::fopen(filename.c_str(), "w");
                        if (!file) return nullptr;

                        ::fclose(file);

                        file = ::fopen(filename.c_str(), "rb+");
                        if (!file) return nullptr;
                    }
                }

                return file;
            }

        public:
            MultiFile(
                const std::filesystem::path &base_filepath,
                size_t max_size_per_file
            )
            :
                io::Object(base_filepath),
                __max_size_per_file(max_size_per_file)
            {}

            int fopen(io::IOMode mode)
            {
                this->fclose();

                this->__data_size = 0;

                std::string base_filepath = this->_filepath.generic_string() + "_";
                uint64_t index = 0;
                std::string single_filepath;
                FILE *temp_file = nullptr;
                switch (mode)
                {
                    case io::IOMode::NEW_EMPTY_AND_WRITE:
                        // Remove all
                        for (index = 1; ; index++)
                        {
                            single_filepath = base_filepath + std::to_string(index);

                            if (!std::filesystem::is_regular_file(single_filepath)) break;

                            std::filesystem::remove(single_filepath);
                        }

                        // Create and open filename_0
                        single_filepath = base_filepath + "0";
                        temp_file = ::fopen(single_filepath.c_str(), "w");
                        if (!temp_file) return EOF;

                        this->__files.push_back(temp_file);
                        break;

                    case io::IOMode::KEEP_EXISTING_AND_READ_WRITE:
                        // Get all
                        for (index = 0; ; index++)
                        {
                            single_filepath = base_filepath + std::to_string(index);

                            if (!std::filesystem::is_regular_file(single_filepath)) break;

                            // Open each file
                            temp_file = ::fopen(single_filepath.c_str(), "rb+");
                            if (!temp_file) return EOF;

                            this->__files.push_back(temp_file);

                            if (::fseek(temp_file, 0, SEEK_END) != 0) return EOF;
                            this->__data_size += ::ftell(temp_file);
                        }
                        break;
                }

                this->__cur_index = 0;

                return 0;
            }

            int fseek(int64_t offset, int origin)
            {
                int64_t cur_index = 0;
                switch (origin)
                {
                    case SEEK_SET:
                        cur_index = 0;
                        break;
                    case SEEK_CUR:
                        cur_index = this->__cur_index;
                        break;
                    case SEEK_END:
                        cur_index = this->__data_size;
                        break;
                }

                int64_t index = cur_index + offset;

                if (index > this->__data_size) return EOF;
                if (index < 0) return EOF;

                this->__cur_index = index;

                return 0;
            }

            long ftell() const
            {
                return this->__cur_index;
            }

            int fgetc()
            {
                uint64_t file_index = this->__cur_index / this->__max_size_per_file;
                FILE *file = this->_try_to_get_file(file_index);
                if (!file) return EOF;

                ::fseek(file, 0, SEEK_END);
                size_t file_size = ::ftell(file);

                uint64_t file_data_index = this->__cur_index % this->__max_size_per_file;
                if (::fseek(file, file_data_index, SEEK_SET) != 0)
                {
                    return EOF;
                }

                int ret = ::fgetc(file);
                if (ret == EOF) return EOF;

                this->__cur_index++;

                return ret;
            }

            size_t fread(
                void *__restrict__ dst_buffer,
                size_t element_size,
                size_t count
            )
            {
                size_t max_size = element_size * count;

                FILE *file = nullptr;
                uint64_t file_index = 0;
                size_t file_data_size = 0;
                uint64_t file_data_index = 0;
                size_t readed_size = 0;
                size_t data_size_left = 0; 
                size_t ret_readed = 0;
                size_t total_readed = 0;
                for (
                    uint64_t data_index = 0;
                    data_index < max_size && this->__cur_index < this->__data_size;
                )
                {
                    file_index = data_index / this->__max_size_per_file;
                    file = this->_try_to_get_file(file_index);
                    if (!file)
                    {
                        this->__files.push_back(nullptr);
                        file = this->_try_to_get_file(file_index);
                        if (!file) break;
                    }

                    if (::fseek(file, 0, SEEK_END) != 0) break;
                    file_data_size = ::ftell(file);

                    file_data_index = this->__cur_index % this->__max_size_per_file;
                    if (::fseek(file, file_data_index, SEEK_SET) != 0) break;

                    readed_size = file_data_size - file_data_index;

                    data_size_left = max_size - data_index;
                    if (data_size_left < readed_size)
                    {
                        readed_size = data_size_left;
                    }

                    ret_readed = ::fread(
                        reinterpret_cast<unsigned char *>(dst_buffer) + data_index,
                        sizeof(unsigned char),
                        readed_size,
                        file
                    );

                    this->__cur_index += ret_readed;

                    data_index += ret_readed;
                    total_readed += ret_readed;

                    if (ret_readed != readed_size) break;
                }

                return total_readed;
            }

            int fputc(int c)
            {
                uint64_t file_index = this->__cur_index / this->__max_size_per_file;
                FILE *file = this->_try_to_get_file(file_index);
                if (!file)
                {
                    this->__files.push_back(nullptr);
                    file = this->_try_to_get_file(file_index);
                    if (!file) return EOF;
                }

                uint64_t cur_file_data_index = this->__cur_index % this->__max_size_per_file;
                if (::fseek(file, cur_file_data_index, SEEK_SET) != 0)
                {
                    return EOF;
                }

                int ret = ::fputc(c, file);
                if (ret == EOF) return EOF;

                this->__cur_index++;

                if (this->__cur_index >= this->__data_size)
                {
                    this->__data_size = this->__cur_index;
                }

                return ret;
            }

            size_t fwrite(
                const void *__restrict__ data,
                size_t size,
                size_t count
            )
            {
                size_t data_size = size * count;

                FILE *file = nullptr;
                uint64_t file_index = 0;
                uint64_t file_data_index = 0;
                size_t written_size = 0;
                size_t data_size_left = 0;
                size_t ret_written = 0;
                size_t total_written = 0;
                for (uint64_t data_index = 0; data_index < data_size; )
                {

                    file_index = this->__cur_index / this->__max_size_per_file;
                    file = this->_try_to_get_file(file_index);
                    if (!file)
                    {
                        this->__files.push_back(nullptr);
                        file = this->_try_to_get_file(file_index);
                        if (!file) break;
                    }

                    file_data_index = this->__cur_index % this->__max_size_per_file;
                    if (::fseek(file, file_data_index, SEEK_SET) != 0) break;

                    written_size = this->__max_size_per_file - file_data_index;

                    data_size_left = data_size - data_index;
                    if (data_size_left < written_size)
                    {
                        written_size = data_size_left;
                    }

                    ret_written = ::fwrite(
                        reinterpret_cast<const unsigned char *>(data) + data_index,
                        sizeof(unsigned char),
                        written_size,
                        file
                    );

                    this->__cur_index += ret_written;

                    data_index += ret_written;
                    total_written += ret_written;

                    if (ret_written != written_size) break;
                }

                if (this->__cur_index > this->__data_size)
                {
                    this->__data_size = this->__cur_index;
                }

                return total_written;
            }

            int fclose()
            {
                for (auto &__file : this->__files)
                {
                    if (!__file) continue;

                    if (::fclose(__file) == EOF)
                    {
                        return EOF;
                    }
                }

                this->__files.clear();
                this->__cur_index = 0;
                this->__data_size = 0;

                return 0;
            }

            const std::filesystem::path &get_filepath() const
            {
                return this->_filepath;
            }

            bool is_opened() const
            {
                uint64_t file_index = this->__cur_index / this->__max_size_per_file;
                return this->__files[file_index] != nullptr;
            }

            bool is_exists() const
            {
                std::string filepath = this->_filepath.generic_string() + "_0";
                return std::filesystem::is_regular_file(filepath);
            }
    };
}