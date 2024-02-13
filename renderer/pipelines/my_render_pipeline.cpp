// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer/pipelines/my_render_pipeline.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine::renderer
{
    auto MyRenderPipeline::setup(RenderGraphBuilder& builder, core::ref_ptr<Camera> camera) -> void
    {
        builder
            .add_attachment("ForwardColor", camera->get_render_target()->get_width(),
                            camera->get_render_target()->get_height(), RGAttachmentUsage::RenderTarget,
                            camera->get_render_target())
            .add_attachment("Depth", camera->get_render_target()->get_width(),
                            camera->get_render_target()->get_height(), RGAttachmentUsage::DepthStencil)
            .add_graphics_pass(
                "ForwardPass", camera->get_render_target()->get_width(), camera->get_render_target()->get_height(), {},
                {
                    RGAttachmentInfo{"ForwardColor",
                                     RGColorAttachment{rhi::RenderPassLoadOp::Clear, rhi::RenderPassStoreOp::Store,
                                                       math::Color(0.0f, 0.6f, 0.8f, 1.0f)}},
                    RGAttachmentInfo{"Depth", RGDepthStencilAttachment{rhi::RenderPassLoadOp::Clear,
                                                                       rhi::RenderPassStoreOp::Store,
                                                                       rhi::RenderPassLoadOp::DontCare,
                                                                       rhi::RenderPassStoreOp::DontCare, 1.0f, 0x0}},
                },
                [&, camera](RGRenderPassContext& ctx) {
                    struct WorldData
                    {
                        math::Matrixf model;
                        math::Matrixf view;
                        math::Matrixf projection;
                    };

                    auto tasks = *stream | std::views::filter([](auto& element) {
                        return element.mask == (uint8_t)MyRenderMask::Opaque;
                    }) | std::ranges::to<std::vector>();

                    std::ranges::sort(tasks, [](auto const& lhs, auto const& rhs) { return lhs.shader < rhs.shader; });

                    core::ref_ptr<Shader> last_used_shader;

                    for (auto& task : tasks)
                    {
                        if (last_used_shader != task.shader)
                        {
                            ctx.get_command_buffer().set_graphics_pipeline_options(
                                task.shader->get_shader(),
                                rhi::RasterizerStageInfo{.fill_mode = rhi::FillMode::Solid,
                                                         .cull_mode = rhi::CullMode::Back},
                                rhi::BlendColorInfo::Opaque(),
                                rhi::DepthStencilStageInfo{.depth_func = rhi::CompareOp::Less, .depth_write = true});

                            last_used_shader = task.shader;
                        }

                        auto world_data = WorldData{
                            .model = task.model, .view = camera->get_view(), .projection = camera->get_projection()};
                        ctx.bind_buffer<WorldData>("WorldData", world_data, rhi::BufferUsage::ConstantBuffer);
                        ctx.bind_texture("ColorTexture", task.texture->get_texture(),
                                         rhi::TextureUsage::ShaderResource);

                        ctx.get_command_buffer().bind_vertex_buffer(task.drawable->vertex_buffer(), 0,
                                                                    task.drawable->vertex_buffer()->get_size());
                        ctx.get_command_buffer().bind_index_buffer(task.drawable->index_buffer(), 0,
                                                                   task.drawable->index_buffer()->get_size(),
                                                                   rhi::IndexFormat::Uint32);

                        ctx.get_command_buffer().draw_indexed(task.drawable->get_index_count(), 1, 0);
                    }
                });
    }
} // namespace ionengine::renderer