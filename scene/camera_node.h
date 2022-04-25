// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/transform_node.h>
#include <lib/math/matrix.h>

namespace ionengine::scene {

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

class CameraNode : public TransformNode {
public:

    CameraNode();

    float aspect_ratio() const;

    void aspect_ratio(float const value);

    float field_of_view() const;

    void field_of_view(float const value);

    virtual void accept(SceneVisitor& visitor);

private:

    lib::math::Matrixf _view;
    lib::math::Matrixf _projection;

    float _aspect_ratio;
    float _field_of_view;
};

}
