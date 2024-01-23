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
    uint32_t const width,
    uint32_t const height,
    rhi::CommandBuffer& command_buffer
) : 
    inputs(&inputs),
    outputs(&outputs),
    attachments(&attachments),
    resource_cache(&resource_cache),
    width(width),
    height(height),
    command_buffer(&command_buffer)
{

}

/*
auto RGRenderPassContext::get_resource_by_index(uint32_t const index) -> RGResource {
    
    auto result = inputs->find(index);
    assert(result != inputs->end());

    RGAttachment& attachment = attachments->at(result->second);

    RGResource resource;
    if(!attachment.texture) {
        resource = resource_cache->get(result->second, attachment.format, attachment.sample_count, width, height);
    } else {
        resource = RGResource { .texture = attachment.texture };
    }
    return resource;
}

auto RGRenderPassContext::blit(RGResource const& source) -> void {

}

auto RGRenderPassContext::draw(RenderTask const& render_task) -> void {

    wgpu::RenderPipeline render_pipeline{nullptr};
    {
        auto buffer_layout = wgpu::VertexBufferLayout {};
        buffer_layout.attributes = render_task.primitive->attributes.data();
        buffer_layout.attributeCount = render_task.primitive->attributes.size();
        buffer_layout.arrayStride = 32;
        buffer_layout.stepMode = wgpu::VertexStepMode::Vertex;

        auto vertex_state = wgpu::VertexState {};
        vertex_state.module = render_task.shader->get_shader_module();
        vertex_state.entryPoint = "vs_main";
        vertex_state.bufferCount = 1;
        vertex_state.buffers = &buffer_layout;

        auto color_blend_component = wgpu::BlendComponent {};
        color_blend_component.operation = wgpu::BlendOperation::Add;
        color_blend_component.srcFactor = wgpu::BlendFactor::One;
        color_blend_component.dstFactor = wgpu::BlendFactor::One;

        auto blend_state = wgpu::BlendState {};
        blend_state.color = color_blend_component;
        blend_state.alpha = color_blend_component;

        uint32_t target_state_count = 0;
        std::array<wgpu::ColorTargetState, 16> target_states;
        for(auto const& [_, value] : *outputs) {
            RGAttachment const& attachment = attachments->at(value);

            if(auto const* data = std::get_if<RGAttachment::DepthStencilAttachment>(&attachment.attachment)) {
                continue;
            }

            target_states[target_state_count].format = attachment.format;
            target_states[target_state_count].blend = &blend_state;
            
            target_state_count ++;
        }

        auto fragment_state = wgpu::FragmentState {};
        fragment_state.module = render_task.shader->get_shader_module();
        fragment_state.entryPoint = "fs_main";
        fragment_state.targetCount = target_state_count;
        fragment_state.targets = target_states.data();

        auto primitive_state = wgpu::PrimitiveState {};
        primitive_state.topology = wgpu::PrimitiveTopology::TriangleList;
        primitive_state.cullMode = wgpu::CullMode::Front;
        primitive_state.frontFace = wgpu::FrontFace::CCW;

        auto multisample_state = wgpu::MultisampleState {};
        multisample_state.setDefault();

        wgpu::PipelineLayout pipeline_layout{nullptr};
        {
            auto descriptor = wgpu::PipelineLayoutDescriptor {};
            descriptor.bindGroupLayoutCount = 0;
            
            pipeline_layout = context->get_device().createPipelineLayout(descriptor);
        }

        auto descriptor = wgpu::RenderPipelineDescriptor {};
        descriptor.layout = pipeline_layout;
        descriptor.vertex = vertex_state;
        descriptor.fragment = &fragment_state;
        descriptor.primitive = primitive_state;
        descriptor.depthStencil = nullptr;
        descriptor.multisample = multisample_state;

        render_pipeline = context->get_device().createRenderPipeline(descriptor);
    }

    encoder->setPipeline(render_pipeline);

    auto vertex_buffer = render_task.primitive->get_vertex_buffer();
    auto index_buffer = render_task.primitive->get_index_buffer();

    encoder->setVertexBuffer(0, vertex_buffer.buffer->get_buffer(), vertex_buffer.offset, vertex_buffer.size);
    encoder->setIndexBuffer(index_buffer.buffer->get_buffer(), wgpu::IndexFormat::Uint32, index_buffer.offset, index_buffer.size);
    encoder->drawIndexed(render_task.index_count, 1, 0, 0, 0);
}
*/

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

}

auto RenderGraph::execute() -> void {

    auto swapchain_buffer = device->request_next_swapchain_buffer();

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

                command_buffer->begin_render_pass(data.colors, data.depth_stencil);

                RGRenderPassContext ctx(
                    step.inputs,
                    step.outputs,
                    attachments,
                    resource_cache,
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