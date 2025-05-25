// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/quaternion.hpp"
#include "core/ref_ptr.hpp"
#include "core/vector.hpp"

namespace ionengine
{
    class Entity : public core::ref_counted_object
    {
      public:
        Entity();

        virtual ~Entity() = default;

        core::Vec3f position;
        core::Quatf rotation;
        core::Vec3f scale;
    };
} // namespace ionengine