// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    class Exception : public std::exception
    {
      public:
        Exception(std::string_view const error) : message(error)
        {
        }

        const char* what() const noexcept
        {
            return reinterpret_cast<const char*>(message.c_str());
        }

      private:
        std::string message;
    };
} // namespace ionengine::core