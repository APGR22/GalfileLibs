#pragma once

#include <list>
#include <string>

namespace galfile::helper
{
    void split_string_by(
        std::list<std::string> &dst_splits,
        const std::string &str,
        char by_c,
        bool include_empty = false
    )
    {
        dst_splits.clear();

        std::string substr;
        for (char c : str)
        {
            if (c == by_c)
            {
                if (!substr.empty() && !include_empty)
                {
                    dst_splits.push_back(substr);
                    substr.clear();
                }

                continue;
            }

            substr += c;
        }

        if (!substr.empty() && !include_empty)
        {
            dst_splits.push_back(substr);
            substr.clear();
        }
    }
}