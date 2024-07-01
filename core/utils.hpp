// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename... Args>
    class VariantMatcher
    {
      public:
        VariantMatcher(std::variant<Args...> object) : object(object)
        {
        }

        template <typename Type>
        auto case_(std::function<void(Type&)> callback) -> VariantMatcher&
        {
            if (auto value = std::get_if<Type>(&object))
            {
                callback(*value);
            }
            return *this;
        }

      private:
        std::variant<Args...> object;
    };

    template <typename... Args>
    inline auto variant_match(std::variant<Args...> object) -> VariantMatcher<Args...>
    {
        return VariantMatcher(object);
    }
} // namespace ionengine::core

#ifndef DECLARE_ENUM_CLASS_BIT_FLAG
#define DECLARE_ENUM_CLASS_BIT_FLAG(EnumClass)                                                                         \
    using EnumClass##Flags = uint32_t;                                                                                 \
    inline auto operator|(EnumClass const lhs, EnumClass const rhs)->EnumClass##Flags                                  \
    {                                                                                                                  \
        return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);                                                \
    }                                                                                                                  \
    inline auto operator|(EnumClass##Flags const lhs, EnumClass const rhs)->EnumClass##Flags                           \
    {                                                                                                                  \
        return lhs | static_cast<uint32_t>(rhs);                                                                       \
    }                                                                                                                  \
    inline auto operator&(EnumClass const lhs, EnumClass const rhs)->bool                                              \
    {                                                                                                                  \
        return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);                                                \
    }                                                                                                                  \
    inline auto operator&(EnumClass##Flags const lhs, EnumClass const rhs)->bool                                       \
    {                                                                                                                  \
        return lhs & static_cast<uint32_t>(rhs);                                                                       \
    }
#endif // DECLARE_ENUM_CLASS_BIT_FLAG