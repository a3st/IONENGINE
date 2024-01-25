// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/renderer.hpp"
#include "platform/window.hpp"
#include "core/exception.hpp"
#include "math/matrix.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Renderer::Renderer(core::ref_ptr<RenderPipeline> render_pipeline, platform::Window const& window) :
    device(rhi::Device::create(rhi::BackendType::DirectX12, window)),
    render_pipeline(render_pipeline),
    //texture_cache(context),
    //primitive_cache(context, primitive_allocator_2),
    width(window.get_width()),
    height(window.get_height())
{
    primitive_allocator = device->create_allocator(
        4 * 1024 * 1024, 
        64 * 1024 * 1024, 
        (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::Index)
    );

    std::vector<uint8_t> trash_data(1024);

    auto future = device->create_buffer(
        &primitive_allocator, 
        16 * 1024 * 1024, 
        (rhi::BufferUsageFlags)rhi::BufferUsage::Vertex,
        trash_data
    );

    test_buffer = future.get();

    std::vector<uint8_t> shader_bytes;
    {
        std::ifstream ifs("shaders/basic.bin", std::ios::binary);
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        shader_bytes.resize(size);
        ifs.read(reinterpret_cast<char* const>(shader_bytes.data()), size);
    }

    auto shader = device->create_shader(shader_bytes);
}

auto Renderer::update(float const dt) -> void {

    // primitive_cache.update(dt);
    // texture_cache.update(dt);
}

auto Renderer::render() -> void {

    if(!is_graph_initialized) {
        RenderGraphBuilder builder(&device);
        {
            /*std::vector<RGAttachment> inputs;
            for(auto& target : targets) {
                target->resize(width, height);

                if(target->is_custom_render_target()) {
                    render_pipeline->setup(builder, target, width, height, render_tasks);
                } else {
                    inputs = render_pipeline->setup(builder, target, width, height, render_tasks);
                }
            }*/
            
            std::vector<RGAttachment> outputs = {
                RGAttachment::swapchain(rhi::RenderPassLoadOp::Clear, rhi::RenderPassStoreOp::Store, math::Color(0.5f, 0.5f, 0.5f, 1.0f))
            };

            builder.add_graphics_pass(
                "present_pass",
                width,
                height,
                {},
                outputs,
                [&](RGRenderPassContext& ctx) {

                    struct WorldData {
                        math::Matrixf model;
                        math::Matrixf view;
                        math::Matrixf projection;
                    };
                    auto world_data = WorldData { };

                    //ctx.bind_buffer<WorldData>("WorldData", world_data);
                    ctx.get_command_buffer().bind_descriptor("WorldData", test_buffer);
                }
            );
        }
        render_graph = builder.build();
        is_graph_initialized = true;
    }
    
    render_graph->execute();
    //render_tasks.clear();
}

auto Renderer::resize(uint32_t const width, uint32_t const height) -> void {

    this->width = width;
    this->height = height;
    is_graph_initialized = false;
    render_graph = nullptr;
    
    device->resize_swapchain_buffers(width, height);
}

/*
auto Renderer::load_shaders(std::span<ShaderData const> const shaders) -> bool {

    for(auto const& shader : shaders) {
        shader_cache.get(shader);
    }
    return true;
}

auto Renderer::create_camera(CameraProjectionType const projection_type) -> core::ref_ptr<Camera> {

    return core::make_ref<Camera>(context, projection_type);
}

*/
/*
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
}*/