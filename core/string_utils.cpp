// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "string_utils.hpp"
#include "precompiled.h"

namespace ionengine::core::string_utils
{
    auto trim(std::string& source, trim_mode const trim_mode) -> void
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

    auto stob(std::string_view const source) -> std::expected<bool, error>
    {
        int32_t output;
        auto result = std::from_chars(source.data(), source.data() + source.size(), output);

        if (result.ec == std::errc::invalid_argument)
        {
            return std::unexpected(error(error_code::invalid_argument));
        }
        else
        {
            return output;
        }
    }
} // namespace ionengine::core