// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    template <typename Type>
    struct Rect
    {
        Type x;
        Type y;
        Type width;
        Type height;

        Rect(Type const x, Type const y, Type const width, Type const height) : x(x), y(y), width(width), height(height)
        {
        }

        Rect(Rect const& other) : x(other.x), y(other.y), width(other.width), height(other.height)
        {
        }

        auto operator=(Rect const& other) -> Rect&
        {
            x = other.x;
            y = other.y;
            width = other.width;
            height = other.height;
            return *this;
        }
    };

    using Rectf = Rect<float>;
    using Recti = Rect<int32_t>;
} // namespace ionengine::core