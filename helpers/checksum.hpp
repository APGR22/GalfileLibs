#pragma once

#include <string>
#include <openssl/sha.h>
#include "hex.hpp"

namespace galfile::helper
{
    inline std::string checksum(const unsigned char *data, size_t size)
    {
        unsigned char buffer[SHA256_DIGEST_LENGTH];

        SHA256(data, size, buffer);

        std::string hash_string;
        for (uint64_t index = 0; index < sizeof(buffer); index++)
        {
            hash_string += helper::hex(buffer[index]);
        }

        return hash_string;
    }
}