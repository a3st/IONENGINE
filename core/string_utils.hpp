// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"

namespace ionengine::core
{
    /*!
        \brief Modes that used when trim
    */
    enum class trim_mode
    {
        both,
        beg,
        end
    };

    /*!
        \brief Trim string
        \param[in|out] source Input string that will modified
        \param[in] trim_mode Mode that will used to trim
    */
    auto trim(std::string& source, trim_mode const trim_mode) -> void;

    /*!
        \brief Convert string_view to number
        \param[in] source Input string_view that will converted to number
        \return Number or error
    */
    template <typename Type>
    inline auto ston(std::string_view const source) -> std::expected<Type, error>
    {
        Type output;
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

    /*!
        \brief Convert string_view to boolean
        \param[in] source Input string_view that will converted to boolean
        \return Boolean or error
    */
    auto stob(std::string_view const source) -> std::expected<bool, error>;
} // namespace ionengine::core