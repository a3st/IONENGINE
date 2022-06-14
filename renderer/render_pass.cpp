// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/render_pass.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<RenderPass, lib::Result<RenderPassError>> RenderPass::create(
    backend::Device& device,
    std::optional<std::span<ResourcePtr<GPUTexture> const>> const colors,
    std::optional<std::span<RenderPassColorDesc const>> const color_descs,
    ResourcePtr<GPUTexture> const depth_stencil,
    std::optional<RenderPassDepthStencilDesc> const depth_stencil_desc
) {
    
    
    /*std::vector<ResourcePtr<GPUTexture>> color_attachments;
    std::vector<backend::RenderPassLoadOp> color_ops;
    std::vector<lib::math::Color> color_clears;
    std::vector<ResourcePtr<GPUTexture>> input_attachments;

    for(auto const& color : colors.value()) {
        color_attachments.emplace_back(color.attachment);
        color_ops.emplace_back(color.load_op);
        color_clears.emplace_back(color.clear_color);
    }

    if(inputs.has_value()) {
        for(auto const& input : inputs.value()) {
            input_attachments.emplace_back(input.attachment);
        }
    }

    auto depth_stencil_info = depth_stencil.value_or(
        CreateDepthStencilInfo { 
            .attachment = nullptr, 
            .load_op = backend::RenderPassLoadOp::DontCare,
            .clear_depth = 0.0f, 
            .clear_stencil = 0x0
        }
    );

    std::string const render_pass_string = std::string(name);

    uint64_t hash = XXHash64::hash(render_pass_string.data(), render_pass_string.size(), 0);
    hash ^= swapchain.texture.index() ^ swapchain.texture.generation();

    auto render_pass = RenderPass {
        .name = std::move(render_pass_string),
        .width = width,
        .height = height,
        .color_attachments = color_attachments,
        .color_ops = color_ops,
        .color_clears = color_clears,
        .ds_attachment = depth_stencil_info.attachment,
        .ds_op = depth_stencil_info.load_op,
        .ds_clear = { depth_stencil_info.clear_depth, depth_stencil_info.clear_stencil },
        .inputs = input_attachments,
        .func = func,
        .render_pass = create_render_pass(device, color_attachments, color_ops, depth_stencil_info.attachment, depth_stencil_info.load_op, swapchain),
        .hash = hash
    };*/
    return lib::make_expected<RenderPass, lib::Result<RenderPassError>>(std::move(render_pass));
}

/*
backend::Handle<backend::RenderPass> ionengine::renderer::create_render_pass(
    backend::Device& device,
    std::span<ResourcePtr<GPUTexture>> color_attachments,
    std::span<backend::RenderPassLoadOp> color_ops,
    ResourcePtr<GPUTexture> depth_stencil_attachment,
    backend::RenderPassLoadOp depth_stencil_op,
    SwapchainTexture const& swapchain
) {
    std::vector<backend::Handle<backend::Texture>> colors;
    std::vector<backend::RenderPassColorDesc> color_descs;

    assert(color_attachments.size() == color_ops.size() && "color attachments and color ops has different sizes");
    size_t const color_attachments_size = color_attachments.size();

    colors.resize(color_attachments_size);
    color_descs.resize(color_attachments_size);

    for(size_t i = 0; i < color_attachments_size; ++i) {
        if(!color_attachments[i]) {
            colors[i] = swapchain.texture;
        } else {
            colors[i] = color_attachments[i]->get().texture;
        }
        color_descs[i] = backend::RenderPassColorDesc {
            .load_op = color_ops[i],
            .store_op = backend::RenderPassStoreOp::Store
        };
    }

    if(depth_stencil_attachment) {
        backend::Handle<backend::Texture> depth_stencil = depth_stencil_attachment->get().texture;

        auto depth_stencil_desc = backend::RenderPassDepthStencilDesc {
            .depth_load_op = depth_stencil_op,
            .depth_store_op = backend::RenderPassStoreOp::Store,
            .stencil_load_op = depth_stencil_op,
            .stencil_store_op = backend::RenderPassStoreOp::Store
        };

        return device.create_render_pass(colors, color_descs, depth_stencil, depth_stencil_desc);
    } else {
        return device.create_render_pass(colors, color_descs);
    }
}
*/