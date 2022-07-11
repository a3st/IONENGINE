// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::math {

///
/// Color class
///
struct Color {
    float r;
    float g;
    float b;
    float a;

    Color() = default;

    Color(float const _r, float const _g, float const _b, float const _a)
        : r(_r), g(_g), b(_b), a(_a) {}

    Color(Color const& other)
        : r(other.r), g(other.g), b(other.b), a(other.a) {}

    Color(Color&& other) noexcept
        : r(other.r), g(other.g), b(other.b), a(other.a) {}

    Color& operator=(Color const& other) {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    Color& operator=(Color&& other) noexcept {
        r = other.r;
        g = other.g;
        b = other.b;
        a = other.a;
        return *this;
    }

    inline float const* data() const { return &r; }

    inline size_t size() const { return 4; }

    Color& rgb();

    Color& srgb();

    inline bool operator==(Color const& other) const {
        return std::tie(r, g, b, a) ==
               std::tie(other.r, other.g, other.b, other.a);
    }

    inline bool operator!=(Color const& other) const {
        return std::tie(r, g, b, a) !=
               std::tie(other.r, other.g, other.b, other.a);
    }
};

}  // namespace ionengine::math