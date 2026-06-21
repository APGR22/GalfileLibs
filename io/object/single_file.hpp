#pragma once

#include <filesystem>
#include <cstdio>
#include "../object.hpp"

namespace galfile::io::object
{
    class SingleFile : public io::Object
    {
        private:
            FILE *file = nullptr;

        public:
            SingleFile(
                const std::filesystem::path &filepath
            )
            : io::Object(filepath)
            {}

            int fopen(io::IOMode mode)
            {
                switch (mode)
                {
                    case io::IOMode::NEW_EMPTY_AND_WRITE:
                        this->file = ::fopen(
                            this->_filepath.generic_string().c_str(),
                            "w"
                        );
                        break;
                    case io::IOMode::KEEP_EXISTING_AND_READ_WRITE:
                        this->file = ::fopen(
                            this->_filepath.generic_string().c_str(),
                            "rb+"
                        );
                        break;
                }

                if (!this->file) return -1;

                return 0;
            }

            int fseek(int64_t offset, int origin) const
            {
                return ::fseek(this->file, offset, origin);
            }

            long ftell() const
            {
                return ::ftell(this->file);
            }

            int fgetc() const
            {
                return ::fgetc(this->file);
            }

            size_t fread(
                void *__restrict__ dst_buffer,
                size_t element_size,
                size_t count
            ) const
            {
                return ::fread(dst_buffer, element_size, count, this->file);
            }

            int fputc(int c)
            {
                return ::fputc(c, this->file);
            }

            size_t fwrite(
                const void *__restrict__ data,
                size_t size,
                size_t count
            )
            {
                return ::fwrite(data, size, count, this->file);
            }

            int fclose()
            {
                int ret = ::fclose(this->file);
                this->file = nullptr;

                return ret;
            }

            const std::filesystem::path &get_filepath() const
            {
                return this->_filepath;
            }

            bool is_opened() const
            {
                return this->file != nullptr;
            }

            bool is_exists() const
            {
                return std::filesystem::is_regular_file(this->_filepath);
            }
    };
}