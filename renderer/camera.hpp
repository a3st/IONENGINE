// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "render_target.hpp"
#include "math/matrix.hpp"
#include "math/quaternion.hpp"

namespace ionengine {

namespace renderer {

enum class CameraProjectionType {
    Perspective,
    Orthographic
};

class Camera : public core::ref_counted_object {
public:

    Camera(
        rhi::Device& device, 
        core::ref_ptr<rhi::MemoryAllocator> allocator, 
        CameraProjectionType const projection_type, 
        uint32_t const resolution_width, 
        uint32_t const resolution_height
    );

    auto resize(uint32_t const width, uint32_t const height) -> void;

    auto calculate(math::Vector3f const& position, math::Quaternionf const& rotation) -> void;

    auto get_projection() const -> math::Matrixf const& {

        return projection;
    }

    auto get_view() const -> math::Matrixf const& {

        return view;
    }

    auto get_render_target() const -> core::ref_ptr<RenderTarget> {
        
        return render_target;
    }

    auto is_render_to_texture() const -> bool {

        return false;
    }

private:

    rhi::Device* device;
    core::ref_ptr<RenderTarget> render_target{nullptr};

    math::Matrixf view;
    math::Matrixf projection;
    CameraProjectionType projection_type;
    float field_of_view;
    float aspect_ratio;
    float near_dst;
    float far_dst;

    auto create_projection_matrix() -> void;
};

}

}