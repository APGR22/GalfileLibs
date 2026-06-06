#pragma once

#include <string>
#include <cstdint>
#include <algorithm>

namespace galfile::helpers
{
    inline std::string hex(uint64_t bytes)
    {
        if (bytes == 0)
        {
            return "0";
        }

        std::string buffer;

        while (bytes > 0)
        {
            uint8_t c = bytes % 16;

            if (c < 10)
            {
                buffer += '0' + c;
            }
            else
            {
                c -= 10;
                buffer += 'a' + c;
            }

            bytes /= 16;
        }

        std::reverse(buffer.begin(), buffer.end());

        return buffer;
    }
}