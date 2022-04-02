// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/node.h>
#include <scene/transform.h>
#include <lib/math/matrix.h>

namespace ionengine::scene {

class TransformNode : public Node {

    friend class Scene;

public:

    TransformNode() { }

    virtual ~TransformNode() = default;

    void transform(Transform const& transform);

    Transform const& transform() const;

private:

    lib::math::Matrixf _model_local;
    lib::math::Matrixf _model_global;

    Transform _transform;
};

}