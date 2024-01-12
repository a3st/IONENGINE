// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "math/matrix.hpp"

namespace ionengine {

namespace renderer {

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

class Camera : public core::ref_counted_object {
public:

    Camera();

    auto calculate() -> void;

    auto get_projection() -> math::Matrixf const& {

        return projection;
    }

    auto get_view() -> math::Matrixf const& {

        return view;
    }

private:

    math::Matrixf view;
    math::Matrixf projection;
    CameraProjectionType projection_type;
    float field_of_view;

    auto create_projection_matrix() -> void;

};

}

}