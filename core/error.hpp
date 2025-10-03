// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    class error
    {
      public:
        error(std::string_view const message) : _message(message), _stack_trace(std::stacktrace::current())
        {
        }

        auto what() const noexcept -> std::string const&
        {
            return _message;
        }

        auto trace() const noexcept -> std::stacktrace const&
        {
            return _stack_trace;
        }

      private:
        std::string _message;
        std::stacktrace _stack_trace;
    };
} // namespace ionengine::core