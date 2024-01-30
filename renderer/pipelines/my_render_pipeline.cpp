// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/pipelines/my_render_pipeline.hpp"
#include "platform/window.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

auto MyRenderPipeline::setup(
    RenderGraphBuilder& builder, 
    core::ref_ptr<Camera> camera,
    RenderTaskStream& render_task_stream,
    core::ref_ptr<rhi::Shader> test_shader
) -> std::vector<RGAttachment> 
{   
    auto camera_buffer = camera->get_render_target()->get_next_buffer();

    std::vector<RGAttachment> outputs = {
        RGAttachment::External(
            camera_buffer,
            RGAttachment::ColorAttachment {
                rhi::RenderPassLoadOp::Clear, 
                rhi::RenderPassStoreOp::Store, 
                math::Color(0.0f, 0.5f, 0.8f, 1.0f)
            }
        ),
        RGAttachment::DepthStencil(
            "depth",
            rhi::TextureFormat::D32_FLOAT,
            camera_buffer->get_width(),
            camera_buffer->get_height(),
            rhi::RenderPassLoadOp::Clear, 
            rhi::RenderPassStoreOp::Store, 
            rhi::RenderPassLoadOp::DontCare, 
            rhi::RenderPassStoreOp::DontCare
        )
    };

    builder.add_graphics_pass(
        "forward_pass",
        camera_buffer->get_width(),
        camera_buffer->get_height(),
        {},
        outputs,
        [test_shader, &render_task_stream, camera](RGRenderPassContext& ctx) {
            struct WorldData {
                math::Matrixf model;
                math::Matrixf view;
                math::Matrixf projection;
            };

            struct MaterialData {
                uint32_t colorTexture;
            };
            
            ctx.get_command_buffer().set_graphics_pipeline_options(
                test_shader,
                rhi::RasterizerStageInfo {
                    .fill_mode = rhi::FillMode::Solid,
                    .cull_mode = rhi::CullMode::Back
                },
                rhi::BlendColorInfo::Opaque(),
                rhi::DepthStencilStageInfo {
                    .depth_func = rhi::CompareOp::Less,
                    .depth_write = true
                }
            );

            auto tasks = render_task_stream | 
                std::views::filter([](auto& element) { return element.mask == (uint8_t)MyRenderMask::Opaque; }) |
                std::ranges::to<std::vector>();

            std::ranges::sort(tasks, [](auto const& lhs, auto const& rhs) { return lhs.shader < rhs.shader; });

            for(auto const& task : tasks) {
                auto world_data = WorldData {
                    .model = task.model,
                    .view = camera->get_view(),
                    .projection = camera->get_projection()
                };
                ctx.bind_buffer<WorldData>("WorldData", world_data, rhi::BufferUsage::ConstantBuffer);

                ctx.bind_texture("ColorTexture", task.texture->get_texture(), rhi::TextureUsage::ShaderResource);

                auto vertex_buffer = task.drawable->vertex_buffer();
                ctx.get_command_buffer().bind_vertex_buffer(vertex_buffer, 0, vertex_buffer->get_size());

                auto index_buffer = task.drawable->index_buffer();
                ctx.get_command_buffer().bind_index_buffer(index_buffer, 0, index_buffer->get_size(), rhi::IndexFormat::Uint32);

                ctx.get_command_buffer().draw_indexed(task.drawable->get_index_count(), 1, 0);
            }
        }
    );
    return outputs;
}