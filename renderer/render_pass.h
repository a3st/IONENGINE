// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <renderer/gpu_texture.h>
#include <renderer/resource_ptr.h>
#include <renderer/command_pool.h>
#include <lib/math/color.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class RenderPassError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

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
};

struct RenderPass {
    backend::Handle<backend::RenderPass> render_pass;
    uint64_t hash;

    static lib::Expected<RenderPass, lib::Result<RenderPassError>> create(
        backend::Device& device,
        std::string_view const name,
        std::optional<std::span<CreateColorInfo const>> const colors,
        std::optional<std::span<CreateInputInfo const>> const inputs,
        std::optional<CreateDepthStencilInfo> const depth_stencil
    );
};

template<>
struct GPUResourceDeleter<RenderPass> {
    void operator()(backend::Device& device, RenderPass const& render_pass) const {
        device.delete_render_pass(render_pass.render_pass);
    }
};

backend::Handle<backend::RenderPass> create_render_pass(
    backend::Device& device,
    std::span<ResourcePtr<GPUTexture>> color_attachments,
    std::span<backend::RenderPassLoadOp> color_ops,
    ResourcePtr<GPUTexture> depth_stencil_attachment,
    backend::RenderPassLoadOp depth_stencil_op,
    SwapchainTexture const& swapchain
);

}
