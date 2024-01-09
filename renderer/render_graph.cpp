// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "render_graph.hpp"
#include "backend.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

RGResourceCache::RGResourceCache(Backend& backend) : backend(&backend) { 

}

auto RGResourceCache::get(wgpu::TextureFormat const format, uint32_t const sample_count, uint32_t const width, uint32_t const height) -> RGResource {

    auto entry = entries.find({ format, sample_count, width, height });

    if(entry != entries.end()) {
        RGResource resource = entry->second.front();
        entry->second.pop();

        return resource;
    } else {
        
    }
        
    return {};
}

RenderGraph::RenderGraph(Backend& backend) : backend(&backend), resource_cache(backend) {
    
}

auto RenderGraph::execute() -> void {

    auto cur_frame = backend->get_swapchain().getCurrentTextureView();

    wgpu::CommandEncoder encoder{nullptr};
    {
        auto descriptor = wgpu::CommandEncoderDescriptor {};
        encoder = backend->get_device().createCommandEncoder(descriptor);
    }

    {
        auto attachment = wgpu::RenderPassColorAttachment {};
        attachment.view = cur_frame;
        attachment.loadOp = wgpu::LoadOp::Clear;
        attachment.storeOp = wgpu::StoreOp::Store;
        attachment.clearValue = wgpu::Color(0.4, 0.2, 0.3, 1.0);

        auto texture = backend->get_swapchain().getCurrentTexture();

        auto descriptor = wgpu::RenderPassDescriptor {};
        descriptor.colorAttachmentCount = 1;
        descriptor.colorAttachments = &attachment;
        auto render_pass = encoder.beginRenderPass(descriptor);
        render_pass.setViewport(0, 0, texture.getWidth(), texture.getHeight(), 0.0, 1.0);
        render_pass.end();
    }

    wgpu::CommandBuffer command_buffer{nullptr};
    {
        auto descriptor = wgpu::CommandBufferDescriptor {};
        command_buffer = encoder.finish(descriptor);
    }

    backend->get_queue().submit(1, &command_buffer);
    
    backend->get_swapchain().present();

    frame_index = (frame_index + 1) % 2;
}

RenderGraphBuilder::RenderGraphBuilder(Backend& backend) : backend(&backend) {

}

auto RenderGraphBuilder::add_graphics_pass(
    std::string_view const pass_name,
    uint32_t const width,
    uint32_t const height,
    std::span<RGAttachment const> const inputs,
    std::span<RGAttachment const> const outputs
) -> RenderGraphBuilder& {

    return *this;
}

auto RenderGraphBuilder::build(uint32_t const frame_count) -> core::ref_ptr<RenderGraph> {

    return core::make_ref<RenderGraph>(*backend);
}