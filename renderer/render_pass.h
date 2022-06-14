// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <renderer/resource_ptr.h>
#include <lib/math/color.h>
#include <lib/expected.h>

namespace ionengine::renderer {

struct GPUTexture;

enum class RenderPassError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

/*
struct CreateColorInfo {
    ResourcePtr<GPUTexture> attachment;
    backend::RenderPassLoadOp load_op;
    lib::math::Color clear_color;
};

struct CreateInputInfo {
    ResourcePtr<GPUTexture> attachment;
};

struct CreateDepthStencilInfo {
    ResourcePtr<GPUTexture> attachment;
    backend::RenderPassLoadOp load_op;
    float clear_depth;
    uint8_t clear_stencil;
};

struct SwapchainTexture {
    backend::Handle<backend::Texture> texture;
    backend::MemoryState memory_state;
};*/

struct RenderPass {
    backend::Handle<backend::RenderPass> render_pass;

    static lib::Expected<RenderPass, lib::Result<RenderPassError>> create(
        backend::Device& device,
        std::optional<std::span<ResourcePtr<GPUTexture> const>> const colors,
        std::optional<std::span<backend::RenderPassColorDesc const>> const color_descs,
        ResourcePtr<GPUTexture> const depth_stencil,
        std::optional<backend::RenderPassDepthStencilDesc> const depth_stencil_desc
    );
};

template<>
struct GPUResourceDeleter<RenderPass> {
    void operator()(backend::Device& device, RenderPass const& render_pass) const {
        device.delete_render_pass(render_pass.render_pass);
    }
};

}
