// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "string_utils.hpp"
#include "precompiled.h"
#define NOMINMAX
#define UNICODE
#include <windows.h>

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

    auto to_upper_underscore(std::string_view const source) -> std::string
    {
        std::string result;
        bool last_was_upper = false;

        for (size_t i = 0; i < source.length(); ++i)
        {
            char c = source[i];

            if (std::isupper(c) && i > 0 && !last_was_upper)
            {
                result += '_';
            }

            result += std::toupper(c);
            last_was_upper = std::isupper(c);
        }

        return result;
    }

    auto to_wstring(std::string_view const source) -> std::wstring
    {
        int32_t const length =
            ::MultiByteToWideChar(CP_UTF8, 0, source.data(), static_cast<int32_t>(source.size()), nullptr, 0);
        std::wstring dest(length, '\0');
        ::MultiByteToWideChar(CP_UTF8, 0, source.data(), static_cast<int32_t>(source.size()), dest.data(), length);
        return dest;
    }
} // namespace ionengine::core