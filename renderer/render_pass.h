// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <renderer/resource_ptr.h>
#include <lib/math/color.h>
#include <lib/expected.h>

namespace ionengine::renderer {

struct GPUTexture;
struct CommandList;

enum class RenderPassError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

struct RenderPass {
    backend::Handle<backend::RenderPass> render_pass;

    void begin(
        backend::Device& device,
        CommandList& command_list,
        std::span<lib::math::Color const> const color_clears,
        float clear_depth,
        uint8_t clear_stencil
    );

    void end(backend::Device& device, CommandList& command_list);

    static lib::Expected<RenderPass, lib::Result<RenderPassError>> create(
        backend::Device& device,
        std::span<GPUTexture const*> const colors,
        std::span<backend::RenderPassColorDesc const> const color_descs,
        GPUTexture const* depth_stencil,
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
