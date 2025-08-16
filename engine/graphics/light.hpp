// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/color.hpp"
#include "entity.hpp"

namespace ionengine
{
    class Light : public Entity
    {
      public:
        Light();

        virtual ~Light() = default;

        core::Color color;
    };

    class DirectionalLight : public Entity
    {
      public:
        DirectionalLight();

        core::Vec3f direction;
    };
} // namespace ionengine