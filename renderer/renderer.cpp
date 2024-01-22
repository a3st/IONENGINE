// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/renderer.hpp"
#include "platform/window.hpp"
#include "core/exception.hpp"

#include "shader.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Renderer::Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window) :
    device(rhi::Device::create(rhi::BackendType::DirectX12, window)),
    context(window), 
    render_pipeline(render_pipeline),
    shader_cache(context),
    primitive_allocator_2(
        context, 
        16 * 1024 * 1024, 
        wgpu::BufferUsage::CopyDst | wgpu::BufferUsage::Vertex | wgpu::BufferUsage::Index
    ),
    texture_cache(context),
    primitive_cache(context, primitive_allocator_2),
    width(window.get_width()),
    height(window.get_height())
{
    primitive_allocator = device->create_allocator(
        4 * 1024 * 1024, 
        64 * 1024 * 1024, 
        (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::Index)
    );

    device->create_buffer(
        &primitive_allocator, 
        16 * 1024 * 1024, 
        (rhi::BufferUsageFlags)rhi::BufferUsage::Vertex,
        {}
    );
}

auto Renderer::update(float const dt) -> void {

    context.update();
    // primitive_cache.update(dt);
    texture_cache.update(dt);
}

auto Renderer::render(std::span<core::ref_ptr<Camera>> const targets) -> void {

    /*if(!is_graph_initialized) {
        RenderGraphBuilder builder(context);
        {
            std::vector<RGAttachment> inputs;
            for(auto& target : targets) {
                target->resize(width, height);

                if(target->is_custom_render_target()) {
                    render_pipeline->setup(builder, target, width, height, render_tasks);
                } else {
                    inputs = render_pipeline->setup(builder, target, width, height, render_tasks);
                }
            }
            
            std::vector<RGAttachment> outputs = {
                RGAttachment::swapchain(wgpu::LoadOp::Clear, wgpu::StoreOp::Store, wgpu::Color(0.5, 0.5, 0.5, 1.0))
            };

            builder.add_graphics_pass(
                "present_pass",
                width,
                height,
                inputs,
                outputs,
                [](RGRenderPassContext& ctx) {
                    RGResource resource = ctx.get_resource_by_index(0);
                    ctx.blit(resource);
                }
            );
        }
        render_graph = builder.build();
        is_graph_initialized = true;
    }*/

    auto swapchain = device->request_next_swapchain_buffer();

    auto command_buffer = device->allocate_command_buffer(rhi::CommandBufferType::Graphics);

    std::array<rhi::RenderPassColorInfo, 1> colors = {
        rhi::RenderPassColorInfo {
            .texture = swapchain,
            .load_op = rhi::RenderPassLoadOp::Clear,
            .store_op = rhi::RenderPassStoreOp::Store,
            .clear_color = math::Color(0.2f, 0.3f, 0.2f, 1.0f)
        }
    };

    command_buffer->begin_render_pass(colors, std::nullopt);
    
    command_buffer->end_render_pass();
    command_buffer->close();

    std::array<core::ref_ptr<rhi::CommandBuffer>, 1> submits = {
        command_buffer
    };

    device->submit_command_lists(submits);

    device->present();

    // render_graph->execute(shader_cache);
    render_tasks.clear();
}

auto Renderer::resize(uint32_t const width, uint32_t const height) -> void {

    this->width = width;
    this->height = height;
    is_graph_initialized = false;
    render_graph = nullptr;
    
    device->resize_swapchain_buffers(width, height);
}

auto Renderer::load_shaders(std::span<ShaderData const> const shaders) -> bool {

    for(auto const& shader : shaders) {
        shader_cache.get(shader);
    }
    return true;
}

auto Renderer::create_camera(CameraProjectionType const projection_type) -> core::ref_ptr<Camera> {

    return core::make_ref<Camera>(context, projection_type);
}

auto Renderer::add_render_task(
    PrimitiveData const& data, 
    uint32_t const index_count, 
    std::string_view const shader_name
) -> void 
{
    auto render_task = RenderTask {
        .primitive = primitive_cache.get(data),
        .index_count = index_count,
        .shader = shader_cache.get(ShaderData { .shader_name = shader_name })
    };
    render_tasks.emplace_back(render_task);
}