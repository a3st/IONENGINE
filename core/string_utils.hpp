// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"

namespace ionengine::core::string_utils
{
    /*!
        \brief Modes that will used when trim
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
    inline auto lexical_cast(std::string_view const source) -> std::expected<Type, error>
    {
        if constexpr (std::is_integral_v<Type> && std::is_same_v<Type, bool>)
        {
            bool output;
            std::istringstream(std::string(source)) >> std::boolalpha >> output;
            return output;
        }
        else
        {
            Type output;
            auto result = std::from_chars(source.data(), source.data() + source.size(), output);

            if (result.ec == std::errc::invalid_argument)
            {
                return std::unexpected(error("the result of the conversion is an invalid argument"));
            }
            else
            {
                return output;
            }
        }
    }
} // namespace ionengine::core::string_utils