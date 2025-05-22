// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    struct Color
    {
        float r;
        float g;
        float b;
        float a;

        Color() = default;

        Color(float const r, float const g, float const b, float const a) : r(r), g(g), b(b), a(a)
        {
        }

        Color(Color const& other) : r(other.r), g(other.g), b(other.b), a(other.a)
        {
        }

        Color(Color&& other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a)
        {
        }

        auto operator=(Color const& other) -> Color&
        {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }

        auto operator=(Color&& other) -> Color&
        {
            r = other.r;
            g = other.g;
            b = other.b;
            a = other.a;
            return *this;
        }

        auto data() const -> float const*
        {
            return &r;
        }

        auto size() const -> size_t
        {
            return 4;
        }

        auto rgb() -> Color&;

        auto srgb() -> Color&;

        auto operator==(Color const& other) const -> bool
        {
            return std::make_tuple(r, g, b, a) == std::make_tuple(other.r, other.g, other.b, other.a);
        }
    };
} // namespace ionengine::core

template <>
struct std::hash<ionengine::core::Color>
{
    auto operator()(ionengine::core::Color const& other) const -> size_t
    {
        return std::hash<float>()(other.r) ^ std::hash<float>()(other.g) ^ std::hash<float>()(other.b) ^
               std::hash<float>()(other.a);
    }
};