// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/matrix.h>

namespace ionengine::scene::components {

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

class Camera {
public:

    Camera() = default;

    float aspect_ratio() const;

    Camera& aspect_ratio(float const value);

    float field_of_view() const;

    Camera& field_of_view(float const value);

private:

    lib::math::Matrixf _view;
    lib::math::Matrixf _projection;

    float _aspect_ratio;
    float _field_of_view;
};

}
