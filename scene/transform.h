// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>
#include <lib/math/quaternion.h>

namespace ionengine::scene {

struct Transform {
    lib::math::Vector3f position;
    lib::math::Quaternionf rotation;
    lib::math::Vector3f scale;
};

}