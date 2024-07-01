// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "math/color.hpp"
#include "precompiled.h"

namespace ionengine::math
{
    Color& Color::srgb()
    {
        for (size_t const i : std::views::iota(0u, 3u))
        {
            float color = (&r)[i];
            if (color < 0.0031308f)
            {
                color = color * 12.92f;
            }
            else
            {
                color = 1.055f * std::powf(color, 1.0f / 2.4f) - 0.05499995f;
            }
        }
        return *this;
    }

    Color& Color::rgb()
    {
        for (size_t const i : std::views::iota(0u, 3u))
        {
            float color = (&r)[i];
            if (color <= 0.040448643f)
            {
                color = color / 12.92f;
            }
            else
            {
                color = std::powf((color + 0.055f) / 1.055f, 2.4f);
            }
        }
        return *this;
    }
} // namespace ionengine::math