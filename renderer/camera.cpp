// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "camera.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Camera::Camera() {

}

auto Camera::resize(Backend& backend, uint32_t const width, uint32_t const height) -> void {

    default_render_target = core::make_ref<Texture2D>(
        backend,
        width,
        height,
        1,
        wgpu::TextureFormat::BGRA8Unorm,
        1,
        wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::TextureBinding
    );
}

auto Camera::calculate() -> void {

    
}

auto create_projection_matrix() -> void {
    
}