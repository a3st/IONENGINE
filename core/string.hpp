// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    namespace trim_mode
    {
        inline uint32_t both = 0;
        inline uint32_t beg = 1;
        inline uint32_t end = 2;
    }; // namespace trim_mode

    inline auto trim(std::string& source, uint32_t const trim_mode) -> void
    {
        if (trim_mode == trim_mode::beg || trim_mode == trim_mode::both)
        {
            source.erase(source.begin(), std::find_if(source.begin(), source.end(),
                                                      [](unsigned char ch) { return !std::isspace(ch); }));
        }

        if (trim_mode == trim_mode::end || trim_mode == trim_mode::both)
        {
            source.erase(
                std::find_if(source.rbegin(), source.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
                source.end());
        }
    }
} // namespace ionengine::core