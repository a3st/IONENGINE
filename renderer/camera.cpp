// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "camera.hpp"
#include "context.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Camera::Camera(
    Context& context, 
    CameraProjectionType const projection_type
) : 
    context(&context),
    projection_type(projection_type),
    field_of_view(68.0f),
    near_dst(0.0f),
    far_dst(100.0f)
{

}

auto Camera::resize(uint32_t const width, uint32_t const height) -> void {

    default_render_target = core::make_ref<Texture2D>(
        *context,
        width,
        height,
        1,
        wgpu::TextureFormat::BGRA8Unorm,
        1,
        wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding
    );

    aspect_ratio = static_cast<float>(width / height);
}

auto Camera::calculate(math::Vector3f const& position, math::Quaternionf const& rotation) -> void {

    view = math::Matrixf::look_at_rh(position, position + rotation * math::Vector3f(0.0f, 0.0f, -1.0f), math::Vector3f(0.0f, 1.0f, 0.0f));
    create_projection_matrix();
}

auto Camera::create_projection_matrix() -> void {
    
    if(projection_type == CameraProjectionType::Perspective) {
        projection = math::Matrixf::perspective_rh(field_of_view, aspect_ratio, near_dst, far_dst);
    } else {

    }
}