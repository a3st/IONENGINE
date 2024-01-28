// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/pipelines/my_render_pipeline.hpp"
#include "platform/window.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

auto MyRenderPipeline::setup(
    RenderGraphBuilder& builder, 
    core::ref_ptr<Camera> camera,
    std::span<RenderTask> const render_tasks,
    core::ref_ptr<rhi::Shader> test_shader
) -> std::vector<RGAttachment> 
{   
    auto camera_buffer = camera->get_render_target()->get_next_buffer();

    std::vector<RGAttachment> outputs = {
        RGAttachment::External(
            camera_buffer,
            rhi::RenderPassLoadOp::Clear, 
            rhi::RenderPassStoreOp::Store, 
            math::Color(0.0f, 0.5f, 0.8f, 1.0f)
        )
    };

    builder.add_graphics_pass(
        "forward_pass",
        camera_buffer->get_width(),
        camera_buffer->get_height(),
        {},
        outputs,
        [test_shader, render_tasks](RGRenderPassContext& ctx) {
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
    return outputs;
}