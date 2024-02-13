// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename Type>
    class Singleton
    {
        friend typename Type;

      public:
        Singleton(Singleton const&) = delete;

        auto operator=(Singleton const&) = delete;

        static auto instance() -> Type&
        {
            std::call_once(init_flag, [&]() { ptr = std::make_unique<Type>(); });
            return *ptr;
        }

      private:
        Singleton() = default;

        inline static std::unique_ptr<Type> ptr;
        inline static std::once_flag init_flag;
    };

} // namespace ionengine::core