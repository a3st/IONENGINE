// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <scene/transform.h>

namespace ionengine::scene {

class TransformNode : public SceneNode {

    friend class Scene;

public:

    TransformNode() = default;

    void transform(Transform const& transform);

    Transform const& transform() const;

private:

    lib::math::Matrixf _model_local;
    lib::math::Matrixf _model_global;

    Transform _transform;
};

}