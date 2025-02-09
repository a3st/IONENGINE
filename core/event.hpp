// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename Ret, typename... Args>
    class event;

    template <typename Ret, typename... Args>
    class event<Ret(Args...)>
    {
      public:
        event() = default;

        event(event const&) = delete;

        event(event&& other) : functions(other.functions)
        {
        }

        auto operator=(event const&) -> event& = delete;

        auto operator=(event&& other) -> event&
        {
            functions = std::move(other.functions);
            return *this;
        }

        template <typename Func>
        auto bind(Func&& function) -> void
        {
            functions.emplace_back(function);
        }

        auto unbind_all() -> void
        {
            functions.clear();
        }

        template <typename... CArgs>
        auto invoke(CArgs&&... args) -> void
        {
            for (auto const& function : functions)
            {
                function(std::forward<CArgs>(args)...);
            }
        }

        template <typename Func>
        auto operator+=(Func&& function) -> void
        {
            this->bind(function);
        }

        template <typename... CArgs>
        auto operator()(CArgs&&... args) -> void
        {
            this->invoke(std::forward<CArgs>(args)...);
        }

      private:
        std::vector<std::function<Ret(Args...)>> functions;
    };
} // namespace ionengine::core