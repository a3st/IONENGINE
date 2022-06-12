// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <lib/expected.h>
#include <renderer/gpu_program.h>
#include <renderer/frame_graph.h>
#include <asset/shader.h>

namespace ionengine::renderer {

enum class GPUPipelineError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

struct GPUPipeline {
    backend::Handle<backend::Pipeline> pipeline;

    void bind(backend::Device& device, CommandList& command_list);

    static lib::Expected<GPUPipeline, lib::Result<GPUPipelineError>> create(backend::Device& device, asset::Shader const& shader, GPUProgram const& program, RenderPass const& render_pass);
};

template<>
struct GPUResourceDeleter<GPUPipeline> {
    void operator()(backend::Device& device, GPUPipeline const& pipeline) const {
        device.delete_pipeline(pipeline.pipeline);
    }
};

backend::FillMode constexpr get_shader_fill_mode(asset::ShaderFillMode const fill_mode);

backend::CullMode constexpr get_shader_cull_mode(asset::ShaderCullMode const cull_mode);

}
