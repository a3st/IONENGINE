// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/scene_node.h>
#include <lib/math/matrix.h>
#include <lib/math/vector.h>
#include <lib/math/quaternion.h>

namespace ionengine::scene {

class TransformNode : public SceneNode {

    friend class SceneGraph;

public:

    TransformNode();

    virtual void accept(SceneVisitor& visitor);

    void position(lib::math::Vector3f const& position);

    lib::math::Vector3f const& position() const;

    void rotation(lib::math::Quaternionf const& rotation);

    lib::math::Quaternionf const& rotation() const;

    void scale(lib::math::Vector3f const& scale);

    lib::math::Vector3f const& scale() const;

    lib::math::Matrixf const& transform_local() const;

    lib::math::Matrixf const& transform_global() const;

private:

    lib::math::Matrixf _model_local;
    lib::math::Matrixf _model_global;

    lib::math::Vector3f _position;
    lib::math::Quaternionf _rotation;
    lib::math::Vector3f _scale;
};

}
