// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/render_pass.h>
#include <renderer/gpu_texture.h>
#include <renderer/command_list.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<RenderPass, lib::Result<RenderPassError>> RenderPass::create(
    backend::Device& device,
    std::span<GPUTexture const*> const colors,
    std::span<backend::RenderPassColorDesc const> const color_descs,
    GPUTexture const* depth_stencil,
    std::optional<backend::RenderPassDepthStencilDesc> const depth_stencil_desc
) {
    assert(colors.size() == color_descs.size() && "color attachments and color ops has different sizes");

    std::vector<backend::Handle<backend::Texture>> color_textures;

    color_textures.resize(colors.size());

    for(size_t i = 0; i < colors.size(); ++i) {
        color_textures.at(i) = colors[i]->texture;
    }

    backend::Handle<backend::RenderPass> render_pass_handle;

    if(depth_stencil) {
        backend::Handle<backend::Texture> depth_stencil_texture = depth_stencil->texture;
        render_pass_handle = device.create_render_pass(color_textures, color_descs, depth_stencil_texture, depth_stencil_desc);
    } else {
        render_pass_handle = device.create_render_pass(color_textures, color_descs);
    }

    auto render_pass = RenderPass {
        .render_pass = render_pass_handle
    };
    return lib::make_expected<RenderPass, lib::Result<RenderPassError>>(std::move(render_pass));
}

void RenderPass::begin(
    backend::Device& device,
    CommandList& command_list,
    std::span<lib::math::Color const> const color_clears,
    float clear_depth,
    uint8_t clear_stencil
) {
    device.begin_render_pass(command_list.command_list, render_pass, color_clears, clear_depth, clear_stencil);
}

void RenderPass::end(backend::Device& device, CommandList& command_list) {
    device.end_render_pass(command_list.command_list);
}
