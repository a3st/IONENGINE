// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "render_graph.hpp"
#include "backend.hpp"
#include "core/exception.hpp"

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

RenderGraph::RenderGraph(
    Backend& backend, 
    std::vector<RGRenderPass> const& steps, 
    std::unordered_map<uint64_t, RGResource> const& resources
) : 
    backend(&backend), 
    resource_cache(backend), 
    steps(steps),
    resources(resources)
{

}

auto RenderGraph::execute() -> void {

    auto cur_swapchain_view = backend->get_swapchain().getCurrentTextureView();

    wgpu::CommandEncoder encoder{nullptr};
    {
        auto descriptor = wgpu::CommandEncoderDescriptor {};
        encoder = backend->get_device().createCommandEncoder(descriptor);
    }

    for(auto& step : steps) {
        auto visitor = make_visitor(
            [&](RGRenderPass::GraphicsPass& data) {
                uint32_t start_index = 0;
                
                if(data.depth_stencil.has_value()) {
                    start_index = 1;
                }

                for(uint32_t i = 0; i < data.colors.size(); ++i) {
                    auto result = step.outputs.find(start_index + i);
                    assert(result != step.outputs.end());

                    if(result->second == SWAPCHAIN_NAME) {
                        data.colors[i].view = cur_swapchain_view;
                    } else {
                        // Get from Cache
                    }
                }

                auto descriptor = wgpu::RenderPassDescriptor {};
                descriptor.colorAttachmentCount = data.colors.size();
                descriptor.colorAttachments = data.colors.data();

                auto render_pass = encoder.beginRenderPass(descriptor);
                render_pass.setViewport(0, 0, data.width, data.height, 0.0, 1.0);
                render_pass.end();
            },
            [&](RGRenderPass::ComputePass& data) {
                // TODO!
            }
        );

        std::visit(visitor, step.pass);
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
    std::span<RGAttachment const> const outputs,
    graphics_pass_func_t const& callback
) -> RenderGraphBuilder& {

    std::vector<wgpu::RenderPassColorAttachment> colors;
    std::optional<wgpu::RenderPassDepthStencilAttachment> depth_stencil;

    for(auto const& output : outputs) {
        auto visitor = make_visitor(
            [&](RGAttachment::ColorAttachment const& data) {
                auto attachment = wgpu::RenderPassColorAttachment {};
                attachment.loadOp = data.load_op;
                attachment.storeOp = data.store_op;
                attachment.clearValue = data.clear_color;

                colors.push_back(attachment);
            },
            [&](RGAttachment::DepthStencilAttachment const& data) {
                auto attachment = wgpu::RenderPassDepthStencilAttachment {};
                attachment.depthLoadOp = data.depth_load_op;
                attachment.depthStoreOp = data.depth_store_op;
                attachment.stencilLoadOp = data.stencil_load_op;
                attachment.stencilStoreOp = data.depth_store_op;
                attachment.depthClearValue = data.clear_depth;
                attachment.stencilClearValue = data.clear_stencil;

                depth_stencil = attachment;
            }
        );

        std::visit(visitor, output.attachment);
    }

    auto render_pass = RGRenderPass::graphics(pass_name, width, height, colors, depth_stencil);

    for(uint32_t i = 0; i < inputs.size(); ++i) {
        render_pass.inputs.insert({ i, XXHash64::hash(inputs[i].name.c_str(), inputs[i].name.size(), 0) });
    }

    for(uint32_t i = 0; i < outputs.size(); ++i) {
        render_pass.outputs.insert({ i, XXHash64::hash(outputs[i].name.c_str(), outputs[i].name.size(), 0) });
    }

    steps.push_back(render_pass);
    return *this;
}

auto RenderGraphBuilder::build(uint32_t const frame_count) -> core::ref_ptr<RenderGraph> {

    return core::make_ref<RenderGraph>(*backend, steps, resources);
}