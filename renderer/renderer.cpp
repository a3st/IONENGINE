// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/renderer.hpp"
#include "platform/window.hpp"
#include "core/exception.hpp"

#include "shader.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Renderer::Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window) : 
    backend(window), 
    render_pipeline(render_pipeline),
    shader_cache(backend),
    mesh_allocator(backend, 32 * 1024 * 1024, wgpu::BufferUsage::CopyDst),
    width(window.get_width()),
    height(window.get_height())
{
    
}

auto Renderer::render(std::span<core::ref_ptr<Camera>> const targets) -> void {

    if(!is_graph_initialized) {
        RenderGraphBuilder builder(backend);
        {
            std::vector<RGAttachment> inputs;
            for(auto& target : targets) {
                target->resize(width, height);

                if(target->is_custom_render_target()) {
                    render_pipeline->setup(builder, target, width, height);
                } else {
                    inputs = render_pipeline->setup(builder, target, width, height);
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
    }

    backend.update();
    render_graph->execute(shader_cache);
}

auto Renderer::resize(platform::Window const& window, uint32_t const width, uint32_t const height) -> void {

    this->width = width;
    this->height = height;
    // backend.recreate_swapchain(width, height);
}

auto Renderer::load_shaders(std::span<ShaderData const> const shaders) -> bool {

    for(auto const& shader : shaders) {
        shader_cache.get(shader);
    }
    return true;
}

auto Renderer::create_camera() -> core::ref_ptr<Camera> {

    return core::make_ref<Camera>(backend);
}

auto Renderer::add_render_task(PrimitiveData const& data) -> void {

}