// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <array>
#include <cassert>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <numbers>
#include <numeric>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <semaphore>
#include <shared_mutex>
#include <span>
#include <spanstream>
#include <string>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <variant>
#include <vector>

#ifndef DECLARE_ENUM_CLASS_BIT_FLAG
#define DECLARE_ENUM_CLASS_BIT_FLAG(EnumClass)                                                                         \
    using EnumClass##Flags = uint32_t;                                                                                 \
    inline auto operator|(EnumClass const lhs, EnumClass const rhs) -> EnumClass##Flags                                \
    {                                                                                                                  \
        return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);                                                \
    }                                                                                                                  \
    inline auto operator|(EnumClass##Flags const lhs, EnumClass const rhs) -> EnumClass##Flags                         \
    {                                                                                                                  \
        return lhs | static_cast<uint32_t>(rhs);                                                                       \
    }                                                                                                                  \
    inline auto operator&(EnumClass const lhs, EnumClass const rhs) -> bool                                            \
    {                                                                                                                  \
        return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);                                                \
    }                                                                                                                  \
    inline auto operator&(EnumClass##Flags const lhs, EnumClass const rhs) -> bool                                     \
    {                                                                                                                  \
        return lhs & static_cast<uint32_t>(rhs);                                                                       \
    }
#endif // DECLARE_ENUM_CLASS_BIT_FLAG

namespace utils
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
} // namespace utils