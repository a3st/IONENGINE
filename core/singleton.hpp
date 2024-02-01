// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename Type>
    class Singleton
    {
      public:
        Singleton(Singleton const&) = delete;

        auto operator=(Singleton const&) = delete;

        static auto instance() -> Type&
        {
            static const auto instance = std::make_unique<Type>();
            return *instance;
        }

      private:
        Singleton() = default;
    };

} // namespace ionengine::core