// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "math/matrix.hpp"
#include "math/quaternion.hpp"
#include "texture.hpp"

namespace ionengine {

namespace renderer {

class Context;

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

class Camera : public core::ref_counted_object {
public:

    Camera(Context& context, CameraProjectionType const projection_type);

    auto resize(uint32_t const width, uint32_t const height) -> void;

    auto calculate(math::Vector3f const& position, math::Quaternionf const& rotation) -> void;

    auto get_projection() const -> math::Matrixf const& {

        return projection;
    }

    auto get_view() const -> math::Matrixf const& {

        return view;
    }

    auto get_render_target() const -> core::ref_ptr<Texture> {
        
        if(!render_target) {
            return default_render_target;
        } else {
            return render_target;
        }
    }

    auto is_custom_render_target() const -> bool {

        return render_target;
    }

private:

    Context* context;
    math::Matrixf view;
    math::Matrixf projection;
    CameraProjectionType projection_type;
    float field_of_view;
    float aspect_ratio;
    float near_dst;
    float far_dst;

    core::ref_ptr<Texture> render_target{nullptr};
    core::ref_ptr<Texture> default_render_target{nullptr};

    auto create_projection_matrix() -> void;
};

}

}