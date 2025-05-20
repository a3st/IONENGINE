// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    class error
    {
      public:
        error(std::string_view const message) : message(message), stack_trace(std::stacktrace::current())
        {
        }

        auto what() const noexcept -> std::string const&
        {
            return message;
        }

        auto trace() const noexcept -> std::stacktrace const&
        {
            return stack_trace;
        }

      private:
        std::string message;
        std::stacktrace stack_trace;
    };
} // namespace ionengine::core