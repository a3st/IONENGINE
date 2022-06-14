// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_pipeline.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GPUPipeline, lib::Result<GPUPipelineError>> GPUPipeline::create(backend::Device& device, GPUProgram const& program, asset::ShaderDrawParameters const& draw_parameters, RenderPass const& render_pass) {
    auto gpu_pipeline = GPUPipeline {};

    auto rasterizer_desc = backend::RasterizerDesc {
        .fill_mode = get_shader_fill_mode(draw_parameters.fill_mode),
        .cull_mode = get_shader_cull_mode(draw_parameters.cull_mode)
    };

    auto depth_stencil_desc = backend::DepthStencilDesc {
        .depth_func = backend::CompareOp::Less,
        .write_enable = draw_parameters.depth_stencil
    };

    auto blend_desc = backend::BlendDesc { 
        draw_parameters.blend,
        backend::Blend::SrcAlpha, backend::Blend::InvSrcAlpha, backend::BlendOp::Add, 
        backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add 
    };

    gpu_pipeline.pipeline = device.create_pipeline(
        program.descriptor_layout, 
        program.attributes, 
        program.stages, 
        rasterizer_desc, 
        depth_stencil_desc, 
        blend_desc, 
        render_pass.render_pass, 
        backend::InvalidHandle<backend::CachePipeline>()
    );

    return lib::make_expected<GPUPipeline, lib::Result<GPUPipelineError>>(std::move(gpu_pipeline));
}

void GPUPipeline::bind(backend::Device& device, CommandList& command_list) {
    device.bind_pipeline(command_list.command_list, pipeline);
}

backend::FillMode constexpr ionengine::renderer::get_shader_fill_mode(asset::ShaderFillMode const fill_mode) {
    switch(fill_mode) {
        case asset::ShaderFillMode::Solid: return backend::FillMode::Solid;
        case asset::ShaderFillMode::Wireframe: return backend::FillMode::Wireframe;
        default: {
            assert(false && "invalid data type");
            return backend::FillMode::Solid;
        } 
    }
}

backend::CullMode constexpr ionengine::renderer::get_shader_cull_mode(asset::ShaderCullMode const cull_mode) {
    switch(cull_mode) {
        case asset::ShaderCullMode::Front: return backend::CullMode::Front;
        case asset::ShaderCullMode::Back: return backend::CullMode::Back;
        case asset::ShaderCullMode::None: return backend::CullMode::None;
        default: {
            assert(false && "invalid data type");
            return backend::CullMode::None;
        }
    }
}
