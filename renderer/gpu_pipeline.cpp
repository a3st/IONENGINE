// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_pipeline.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GPUPipeline, lib::Result<GPUPipelineError>> GPUPipeline::create_from_shader(backend::Device& device, Shader const& shader, RenderPass const& render_pass) {
    auto gpu_pipeline = GPUPipeline {};

    gpu_pipeline.pipeline = device.create_pipeline(
        shader.descriptor_layout, 
        shader.attributes, 
        shader.stages, 
        shader.rasterizer, 
        shader.depth_stencil, 
        shader.blend, 
        render_pass.render_pass, 
        backend::InvalidHandle<backend::CachePipeline>()
    );

    return lib::make_expected<GPUPipeline, lib::Result<GPUPipelineError>>(std::move(gpu_pipeline));
}

void GPUPipeline::bind(backend::Device& device, CommandList& command_list) {
    device.bind_pipeline(command_list.command_list, pipeline);
}
