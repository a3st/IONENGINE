// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    enum class error_code
    {
        invalid_argument,
        out_of_range,
        eof,
        parse_token,
        compile
    };

    class error
    {
      public:
        error(error_code const errc, std::string_view const message = "") : errc(errc), message(message)
        {
        }

        auto what() const noexcept -> std::string_view
        {
            return message;
        }

        auto code() const noexcept -> error_code
        {
            return errc;
        }

      private:
        std::string message;
        error_code errc;
    };
} // namespace ionengine::core