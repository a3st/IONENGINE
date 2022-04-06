// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/transform_node.h>
#include <lib/math/matrix.h>

namespace ionengine {

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

class CameraNode : public scene::TransformNode {
public:

    CameraNode() = default;

    float aspect_ratio() const;

    CameraNode& aspect_ratio(float const value);

    float field_of_view() const;

    CameraNode& field_of_view(float const value);

private:

    lib::math::Matrixf _view;
    lib::math::Matrixf _projection;

    float _aspect_ratio;
    float _field_of_view;
};

}
