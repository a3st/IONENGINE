// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"

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

    template <typename Type>
    inline auto ston(std::string_view const source) -> Type
    {
        Type out;
        auto result = std::from_chars(source.data(), source.data() + source.size(), out);

        if (result.ec == std::errc::invalid_argument)
        {
            throw core::runtime_error("An error occurred while converting a number");
        }
        return out;
    }
} // namespace ionengine::core