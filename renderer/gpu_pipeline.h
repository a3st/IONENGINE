// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <renderer/gpu_program.h>
#include <asset/shader.h>
#include <lib/expected.h>

namespace ionengine::renderer {

struct CommandList;
struct RenderPass;

enum class GPUPipelineError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

struct GPUPipeline {
    backend::Handle<backend::Pipeline> pipeline;

    void bind(backend::Device& device, CommandList& command_list);

    static lib::Expected<GPUPipeline, lib::Result<GPUPipelineError>> create(backend::Device& device, GPUProgram const& program, asset::ShaderDrawParameters const& draw_parameters, RenderPass const& render_pass);
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
