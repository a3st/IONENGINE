// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "renderer/pipelines/my_render_pipeline.hpp"
#include "platform/window.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

auto MyRenderPipeline::setup(
    RenderGraphBuilder& builder, 
    //core::ref_ptr<Camera> camera, 
    uint32_t const width, 
    uint32_t const height
    //std::span<RenderTask> const render_tasks
) -> std::vector<RGAttachment> {

    /*std::vector<RGAttachment> inputs = {
        RGAttachment::render_target("albedo", wgpu::TextureFormat::BGRA8Unorm, 1, wgpu::LoadOp::Clear, wgpu::StoreOp::Store),
        RGAttachment::render_target("normal", wgpu::TextureFormat::BGRA8Unorm, 1, wgpu::LoadOp::Clear, wgpu::StoreOp::Store)
    };

    std::vector<RGAttachment> outputs = {
        RGAttachment::external(camera->get_render_target(), wgpu::LoadOp::Clear, wgpu::StoreOp::Store)
    };

    builder.add_graphics_pass(
        "base_pass",
        width,
        height,
        inputs,
        outputs,
        [render_tasks](RGRenderPassContext& ctx) {

            for(auto const& render_task : render_tasks) {
                ctx.draw(render_task);
            }
        }
    );*/

    return {};
}