// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/pipelines/my_render_pipeline.hpp"
#include "renderer/backend.hpp"
#include "platform/window.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

auto MyRenderPipeline::render(Backend& backend, platform::Window const& window) -> core::ref_ptr<RenderGraph> {

    RenderGraphBuilder builder(backend);
    {
        std::vector<RGAttachment> inputs = {
            // RGAttachment::render_target("albedo", wgpu::TextureFormat::BGRA8Unorm, 1, wgpu::LoadOp::Clear, wgpu::StoreOp::Store),
            // RGAttachment::render_target("normal", wgpu::TextureFormat::BGRA8Unorm, 1, wgpu::LoadOp::Clear, wgpu::StoreOp::Store)
        };

        std::vector<RGAttachment> outputs = {
            RGAttachment::swapchain(wgpu::LoadOp::Clear, wgpu::StoreOp::Store, wgpu::Color(0.2, 0.3, 0.1, 1.0))
        };

        builder.add_graphics_pass(
            "pass",
            window.get_width(),
            window.get_height(),
            inputs,
            outputs,
            [&](RGRenderPassContext& ctx) {

                auto renderer = ctx.mesh_renderer;
                

                /*

                auto renderer = ctx.mesh_renderer;
                {
                    auto queue = renderer.get_opaque_queue();
                    ctx.draw(queue);
                }

                */
            }
        );
    }
    return builder.build();
}