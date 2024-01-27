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
    height(window.get_height()),
    primitive_cache(&device)
{
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
                RGAttachment::Swapchain(rhi::RenderPassLoadOp::Clear, rhi::RenderPassStoreOp::Store, math::Color(0.0f, 0.5f, 0.8f, 1.0f))
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
                    auto world_data = WorldData { 
                        .model = math::Matrixf::scale(math::Vector3f(3.0f, 3.0f, 3.0f)),
                        .view = math::Matrixf::look_at_rh(
                            math::Vector3f(5.0f, 0.0, 5.0f), 
                            math::Vector3f(0.0f, 0.0f, 0.0f), 
                            math::Vector3f(0.0f, 1.0f, 0.0f)
                        ),
                        .projection = math::Matrixf::perspective_rh(
                            68.0f * static_cast<float>(std::numbers::pi) / 180.0f,
                            4 / 3,
                            0.1f,
                            100.0f
                        )
                    };

                    ctx.get_command_buffer().set_graphics_pipeline_options(
                        test_shader,
                        rhi::RasterizerStageInfo {
                            .fill_mode = rhi::FillMode::Wireframe,
                            .cull_mode = rhi::CullMode::Front
                        },
                        rhi::BlendColorInfo::Opaque(),
                        std::nullopt
                    );

                    ctx.bind_buffer<WorldData>("WorldData", world_data, rhi::BufferUsage::ConstantBuffer);

                    for(auto const& task : render_tasks) {
                        ctx.get_command_buffer().bind_vertex_buffer(task.drawable.first, 0, task.drawable.first->get_size());
                        ctx.get_command_buffer().bind_index_buffer(task.drawable.second, 0, task.drawable.second->get_size(), rhi::IndexFormat::Uint32);
                        ctx.get_command_buffer().draw_indexed(task.index_count, 1, 0);
                    }
                }
            );
        }
        render_graph = builder.build();
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

auto Renderer::add_render_task(RenderTaskData const& data) -> void {

    auto result = primitive_cache.get(data.primitive);
    if(result) {
        auto render_task = RenderTask {
            .drawable = result.value(),
            .index_count = data.index_count
        };
        render_tasks.emplace_back(render_task);
    }
}