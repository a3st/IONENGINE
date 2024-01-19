// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "render_graph.hpp"
#include "core/exception.hpp"
#include "context.hpp"
#include "shader.hpp"
#include "primitive.hpp"
#include "render_task.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

RGResourceCache::RGResourceCache(Context& context) : context(&context) { 

}

auto RGResourceCache::get_from_entries(
    wgpu::TextureFormat const format, 
    uint32_t const sample_count, 
    uint32_t const width, 
    uint32_t const height
) -> RGResource {

    auto entry = entries.find({ format, sample_count, width, height });

    RGResource resource;

    if(entry != entries.end()) {
        resource = std::move(entry->second.front());
        entry->second.pop();
    } else {
        core::ref_ptr<Texture> texture = core::make_ref<Texture2D>(
            *context,
            width,
            height,
            1,
            format,
            sample_count,
            wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment
        );
        resource = RGResource {
            .texture = texture
        };
    }
    
    return resource;
}

auto RGResourceCache::get(
    uint64_t const id,
    wgpu::TextureFormat const format, 
    uint32_t const sample_count, 
    uint32_t const width, 
    uint32_t const height
) -> RGResource {

    auto result = resources.find(id);

    if(result != resources.end()) {
        if(result->second.has_value()) {
            return std::get<0>(result->second.value());
        } else {
            RGResource resource = get_from_entries(format, sample_count, width, height);
            resources.insert({ id, std::make_tuple(resource, 1) });
            return resource;
        }
    } else {
        RGResource resource = get_from_entries(format, sample_count, width, height);
        resources.insert({ id, std::make_tuple(resource, 1) });
        return resource;
    }
}

auto RGResourceCache::update() -> void {

    for(auto& [key, value] : resources) {
        if(!value.has_value()) {
            continue;
        }

        auto entry = value.value();

        if(std::get<1>(entry) == 0) {
            RGResource resource = std::move(std::get<0>(entry));

            wgpu::TextureFormat const format = resource.texture->get_texture().getFormat();
            uint32_t const sample_count = resource.texture->get_texture().getSampleCount();
            uint32_t const width = resource.texture->get_texture().getWidth();
            uint32_t const height = resource.texture->get_texture().getHeight();

            entries.at({ format, sample_count, width, height }).push(std::move(resource));

            value = std::nullopt;
        } else {
            std::get<1>(entry) --;
        }
    }
}

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

RenderGraph::RenderGraph(
    Context& context, 
    std::vector<RGRenderPass> const& steps,
    std::unordered_map<uint64_t, RGAttachment> const& attachments
) : 
    context(&context), 
    steps(steps),
    resource_cache(context),
    attachments(attachments)
{

}

auto RenderGraph::execute(ShaderCache& shader_cache) -> void {

    auto cur_swapchain_view = context->get_swapchain().getCurrentTextureView();

    wgpu::CommandEncoder encoder{nullptr};
    {
        auto descriptor = wgpu::CommandEncoderDescriptor {};
        encoder = context->get_device().createCommandEncoder(descriptor);
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
                        break;
                    } else {
                        RGAttachment& attachment = attachments.at(result->second);

                        if(!attachment.texture) {
                            RGResource resource = resource_cache.get(result->second, attachment.format, attachment.sample_count, data.width, data.height);
                            data.colors[i].view = resource.texture->get_view();
                        } else {
                            data.colors[i].view = attachment.texture->get_view();
                        }
                    }
                }

                auto descriptor = wgpu::RenderPassDescriptor {};
                descriptor.colorAttachmentCount = data.colors.size();
                descriptor.colorAttachments = data.colors.data();

                auto render_pass = encoder.beginRenderPass(descriptor);
                render_pass.setViewport(0, 0, static_cast<float>(data.width), static_cast<float>(data.height), 0.0, 1.0);

                auto ctx = RGRenderPassContext {
                    .inputs = &step.inputs,
                    .outputs = &step.outputs,
                    .attachments = &attachments,
                    .resource_cache = &resource_cache,
                    .width = data.width,
                    .height = data.height,
                    .encoder = &render_pass,
                    .shader_cache = &shader_cache,
                    .context = context
                };
                data.callback(ctx);

                render_pass.end();
            },
            [&](RGRenderPass::ComputePass& data) {
                // TODO!
            }
        );

        std::visit(visitor, step.pass);

        resource_cache.update();
    }

    wgpu::CommandBuffer command_buffer{nullptr};
    {
        auto descriptor = wgpu::CommandBufferDescriptor {};
        command_buffer = encoder.finish(descriptor);
    }

    context->get_queue().submit(1, &command_buffer);
    
    context->get_swapchain().present();

    frame_index = (frame_index + 1) % 2;
}

RenderGraphBuilder::RenderGraphBuilder(Context& context) : context(&context) {

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

    auto render_pass = RGRenderPass::graphics(callback, pass_name, width, height, colors, depth_stencil);

    for(uint32_t i = 0; i < inputs.size(); ++i) {
        uint64_t const attachment_name = XXHash64::hash(inputs[i].name.c_str(), inputs[i].name.size(), 0);
        render_pass.inputs.insert({ i, attachment_name });
        attachments.insert({ attachment_name, inputs[i] });
    }

    for(uint32_t i = 0; i < outputs.size(); ++i) {
        uint64_t const attachment_name = XXHash64::hash(outputs[i].name.c_str(), outputs[i].name.size(), 0);
        render_pass.outputs.insert({ i, attachment_name });
        attachments.insert({ attachment_name, outputs[i] });
    }

    steps.push_back(render_pass);
    return *this;
}

auto RenderGraphBuilder::build() -> core::ref_ptr<RenderGraph> {

    return core::make_ref<RenderGraph>(*context, steps, attachments);
}