// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/node.h>
#include <lib/math/vector.h>
#include <lib/math/quaternion.h>

namespace ionengine {

class SceneNode : public Node {
public:

    SceneNode() { }

    virtual ~SceneNode() { }

    

private:

    struct Transform {
        lib::math::Vector3f position;
        lib::math::Quaternionf rotation;
        lib::math::Vector3f scale;
    };

    Transform _transform;
};

}