// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math
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
            return std::tie(r, g, b, a) == std::tie(other.r, other.g, other.b, other.a);
        }
    };
} // namespace ionengine::math

template <>
struct std::hash<ionengine::math::Color>
{
    auto operator()(ionengine::math::Color const& other) const -> size_t
    {
        return std::hash<float>()(other.r) ^ std::hash<float>()(other.g) ^ std::hash<float>()(other.b) ^
               std::hash<float>()(other.a);
    }
};