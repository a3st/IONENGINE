// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "render_graph.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

RGRenderPassContext::RGRenderPassContext(
    std::unordered_map<uint32_t, uint64_t>& inputs,
    std::unordered_map<uint32_t, uint64_t>& outputs,
    std::unordered_map<uint64_t, RGAttachment>& attachments,
    RGResourceCache& resource_cache,
    BufferPool& buffer_pool,
    uint32_t const width,
    uint32_t const height,
    rhi::CommandBuffer& command_buffer
) : 
    inputs(&inputs),
    outputs(&outputs),
    attachments(&attachments),
    resource_cache(&resource_cache),
    buffer_pool(&buffer_pool),
    width(width),
    height(height),
    command_buffer(&command_buffer)
{

}

RenderGraph::RenderGraph(
    rhi::Device& device, 
    std::vector<RGRenderPass> const& steps,
    std::unordered_map<uint64_t, RGAttachment> const& attachments
) : 
    device(&device), 
    steps(steps),
    resource_cache(device),
    attachments(attachments)
{
    for(uint32_t i = 0; i < 2; ++i) {
        auto buffer_pool = core::make_ref<BufferPool>(device);
        buffer_pools.emplace_back(buffer_pool);
    }
}

auto RenderGraph::execute() -> void {

    auto swapchain_buffer = device->request_next_swapchain_buffer();
    buffer_pools[frame_index]->reset();

    for(auto& step : steps) {
        auto command_buffer = device->allocate_command_buffer(rhi::CommandBufferType::Graphics);

        auto visitor = make_visitor(
            [&](RGRenderPass::GraphicsPass& data) {
                uint32_t start_index = 0;
                
                if(data.depth_stencil.has_value()) {
                    start_index = 1;
                }

                for(uint32_t i = 0; i < data.colors.size(); ++i) {
                    auto result = step.outputs.find(start_index + i);
                    assert(result != step.outputs.end());

                    RGAttachment& attachment = attachments.at(result->second);

                    if(!attachment.texture) {
                        if(!attachment.is_swapchain) {
                            RGResource resource = resource_cache.get(
                                frame_index, 
                                attachment.format, 
                                attachment.sample_count, 
                                data.width, 
                                data.height, 
                                result->second
                            );
                        } else {
                            data.colors[i].texture = swapchain_buffer;
                        }
                    } else {
                        data.colors[i].texture = attachment.texture;
                    }
                }

                command_buffer->set_viewport(0, 0, data.width, data.height);
                command_buffer->set_scissor(0, 0, data.width, data.height);
                command_buffer->begin_render_pass(data.colors, data.depth_stencil);

                RGRenderPassContext ctx(
                    step.inputs,
                    step.outputs,
                    attachments,
                    resource_cache,
                    &buffer_pools[frame_index],
                    data.width,
                    data.height,
                    &command_buffer
                );
                data.callback(ctx);

                command_buffer->end_render_pass();
                command_buffer->close();
            },
            [&](RGRenderPass::ComputePass& data) {
                // TODO!
            }
        );

        std::visit(visitor, step.pass);

        resource_cache.update();
        submits.emplace_back(command_buffer);
        device->submit_command_lists(submits);
        submits.clear();
    }

    device->present();
    frame_index = (frame_index + 1) % 2;
}

RenderGraphBuilder::RenderGraphBuilder(rhi::Device& device) : device(&device) {

}

auto RenderGraphBuilder::add_graphics_pass(
    std::string_view const pass_name,
    uint32_t const width,
    uint32_t const height,
    std::span<RGAttachment const> const inputs,
    std::span<RGAttachment const> const outputs,
    graphics_pass_func_t const& callback
) -> RenderGraphBuilder& {

    std::vector<rhi::RenderPassColorInfo> colors;
    std::optional<rhi::RenderPassDepthStencilInfo> depth_stencil;

    for(auto const& output : outputs) {
        auto visitor = make_visitor(
            [&](RGAttachment::ColorAttachment const& data) {
                auto attachment = rhi::RenderPassColorInfo {
                    .texture = nullptr,
                    .load_op = data.load_op,
                    .store_op = data.store_op,
                    .clear_color = data.clear_color
                };

                colors.push_back(attachment);
            },
            [&](RGAttachment::DepthStencilAttachment const& data) {
                auto attachment = rhi::RenderPassDepthStencilInfo {
                    .texture = nullptr,
                    .depth_load_op = data.depth_load_op,
                    .depth_store_op = data.depth_store_op,
                    .stencil_load_op = data.stencil_load_op,
                    .stencil_store_op = data.stencil_store_op,
                    .clear_depth = data.clear_depth,
                    .clear_stencil = data.clear_stencil
                };

                depth_stencil = attachment;
            }
        );

        std::visit(visitor, output.attachment);
    }

    auto render_pass = RGRenderPass::graphics(callback, pass_name, width, height, colors, depth_stencil);

    for(uint32_t i = 0; i < inputs.size(); ++i) {
        uint64_t const hash = XXHash64::hash(inputs[i].name.c_str(), inputs[i].name.size(), 0);
        render_pass.inputs.insert({ i, hash });
        attachments.insert({ hash, inputs[i] });
    }

    for(uint32_t i = 0; i < outputs.size(); ++i) {
        uint64_t const hash = XXHash64::hash(outputs[i].name.c_str(), outputs[i].name.size(), 0);
        render_pass.outputs.insert({ i, hash });
        attachments.insert({ hash, outputs[i] });
    }

    steps.push_back(render_pass);
    return *this;
}

auto RenderGraphBuilder::build() -> core::ref_ptr<RenderGraph> {

    return core::make_ref<RenderGraph>(*device, steps, attachments);
}