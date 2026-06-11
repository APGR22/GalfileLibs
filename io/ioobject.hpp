#pragma once

#include <filesystem>
#include <cstdint>

namespace galfile::io
{
    enum IOMode
    {
        NEW_EMPTY_AND_WRITE,
        KEEP_EXISTING_AND_READ_WRITE,
    };

    class IOObject
    {
        protected:
            std::filesystem::path _filepath;

        public:
            IOObject(const std::filesystem::path &filepath)
            : _filepath(filepath)
            {}

            virtual int fopen(IOMode mode) = 0;
            virtual int fseek(int64_t offset, int origin) = 0;
            virtual long ftell() = 0;
            virtual int fgetc() = 0;
            virtual size_t fread(void *__restrict__ dst_buffer, size_t element_size, size_t count) = 0;
            virtual int fputc(int c) = 0;
            virtual size_t fwrite(const void *__restrict__ data, size_t size, size_t count) = 0;
            virtual int fclose() = 0;

            virtual bool is_opened() const = 0;
    };
}