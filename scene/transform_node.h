// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <lib/math/matrix.h>

namespace ionengine::scene {

class TransformNode : public SceneNode {
public:

    TransformNode() { }

    virtual void accept(SceneVisitor& visitor);

private:

    lib::math::Matrixf _model_local;
    lib::math::Matrixf _model_global;
};

}