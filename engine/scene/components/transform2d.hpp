// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/vector.hpp"

namespace ionengine
{
    class Transform2DComponent
    {
      public:
        math::Vec2f position;
        float rotation;
        math::Vec2f scale;

      private:
    };
} // namespace ionengine