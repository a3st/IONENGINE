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
    width(window.get_width()),
    height(window.get_height())
{
    resource_allocator = device->create_allocator(
        512 * 1024,
        128 * 1024 * 1024,
        (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::Index | rhi::BufferUsage::CopyDst | rhi::BufferUsage::ShaderResource)
    );

    primitive_cache = core::make_ref<PrimitiveCache>(&device, resource_allocator);
    texture_cache = core::make_ref<TextureCache>(&device, resource_allocator);

    std::vector<uint8_t> shader_bytes;
    {
        std::ifstream ifs("shaders/basic.bin", std::ios::binary);
        ifs.seekg(0, std::ios::end);
        auto size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        shader_bytes.resize(size);
        ifs.read(reinterpret_cast<char* const>(shader_bytes.data()), size);
    }

    test_shader = device->create_shader(shader_bytes);
}

auto Renderer::update(float const dt) -> void {

}

auto Renderer::render(std::span<core::ref_ptr<Camera>> const targets) -> void {

    if(!is_graph_initialized) {
        RenderGraphBuilder builder;
        {
            std::vector<RGAttachment> inputs;
            for(auto& target : targets) {
                if(target->is_render_to_texture()) {
                    render_pipeline->setup(builder, target, render_tasks, test_shader);
                } else {
                    inputs = render_pipeline->setup(builder, target, render_tasks, test_shader);
                }
            }
        }
        render_graph = builder.build(&device, resource_allocator);
        is_graph_initialized = true;
    }

    render_graph->execute();
    render_tasks.clear();
}

auto Renderer::resize(uint32_t const width, uint32_t const height) -> void {

    this->width = width;
    this->height = height;
    is_graph_initialized = false;
    render_graph = nullptr;
    
    device->resize_swapchain_buffers(width, height);
}

auto Renderer::add_render_tasks(RenderTaskData const& data) -> void {

    auto result = primitive_cache->get(data.primitive);
    if(result) {
        auto render_task = RenderTask {
            .drawable = result.value(),
            .index_count = data.index_count,
            .model = data.model
        };
        render_tasks.emplace_back(render_task);
    }
}

auto Renderer::create_render_target(uint32_t const width, uint32_t const height) -> core::ref_ptr<RenderTarget> {

    return core::make_ref<RenderTarget>(&device, resource_allocator, width, height);
}

auto Renderer::create_camera(
    CameraProjectionType const projection_type, 
    uint32_t const resolution_width,
    uint32_t const resolution_height
) -> core::ref_ptr<Camera> 
{
    return core::make_ref<Camera>(&device, resource_allocator, projection_type, resolution_width, resolution_height);
}