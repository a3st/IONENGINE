// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "camera.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Camera::Camera(
    rhi::Device& device, 
    core::ref_ptr<rhi::MemoryAllocator> allocator, 
    CameraProjectionType const projection_type, 
    uint32_t const resolution_width, 
    uint32_t const resolution_height
) : 
    device(&device),
    projection_type(projection_type),
    field_of_view(68.0f),
    near_dst(0.1f),
    far_dst(100.0f)
{
    render_target = core::make_ref<RenderTarget>(device, allocator, resolution_width, resolution_height);
    aspect_ratio = static_cast<float>(resolution_width / resolution_height);
}

auto Camera::resize(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator, uint32_t const width, uint32_t const height) -> void {
    
    render_target = core::make_ref<RenderTarget>(device, allocator, width, height);
    aspect_ratio = static_cast<float>(width / height);
}

auto Camera::calculate(math::Vector3f const& position, math::Quaternionf const& rotation) -> void {

    //view = math::Matrixf::look_at_rh(position, position + rotation * math::Vector3f(0.0f, 0.0f, -1.0f), math::Vector3f(0.0f, 1.0f, 0.0f));
    view = math::Matrixf::look_at_rh(position,  math::Vector3f(0.0f, 0.0f, 0.0f), math::Vector3f(0.0f, 1.0f, 0.0f));
    create_projection_matrix();
}

auto Camera::create_projection_matrix() -> void {

    if(projection_type == CameraProjectionType::Perspective) {
        projection = math::Matrixf::perspective_rh(field_of_view, aspect_ratio, near_dst, far_dst);
    } else {

    }
}