// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/pipeline.h>

using namespace ionengine;
using namespace ionengine::renderer;

Pipeline::Pipeline(
    backend::Device& device,
    std::span<backend::VertexInputDesc const> const vertex_descs,
    std::shared_ptr<ShaderProgram> shader_program,
    asset::MaterialPassParameters const& parameters,
    backend::Handle<backend::RenderPass> const& render_pass
) : _device(&device), _shader_program(shader_program) {

    backend::RasterizerDesc rasterizer_desc = {
        .fill_mode = get_fill_mode(parameters.fill_mode),
        .cull_mode = get_cull_mode(parameters.cull_mode)
    };

    backend::DepthStencilDesc depth_stencil_desc = { backend::CompareOp::Always, parameters.depth_stencil };

    backend::BlendDesc blend_desc = { 
        false, 
        backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, 
        backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add 
    };

    _pipeline = device.create_pipeline(
        shader_program->descriptor_layout(), 
        vertex_descs, 
        shader_program->shaders(), 
        rasterizer_desc, 
        depth_stencil_desc, 
        blend_desc, 
        render_pass, 
        backend::InvalidHandle<backend::CachePipeline>()
    );
}

std::shared_ptr<Pipeline> Pipeline::from_data(
    backend::Device& device,
    std::shared_ptr<ShaderProgram> shader_program,
    asset::MaterialPassParameters const& parameters,
    backend::Handle<backend::RenderPass> const& render_pass
) {
    std::vector<backend::VertexInputDesc> vertex_inputs;
    vertex_inputs.push_back( backend::VertexInputDesc { "POSITION", 0, backend::Format::RGB32, 0, 0 });
    vertex_inputs.push_back( backend::VertexInputDesc { "TEXCOORD", 0, backend::Format::RG32, 0, sizeof(float) * 3 });
    vertex_inputs.push_back( backend::VertexInputDesc { "NORMAL", 0, backend::Format::RGB32, 0, sizeof(float) * 2 });
    vertex_inputs.push_back( backend::VertexInputDesc { "TANGENT", 1, backend::Format::RGB32, 0, sizeof(float) * 3 });
    vertex_inputs.push_back( backend::VertexInputDesc { "COLOR", 1, backend::Format::RGB32, 0, sizeof(float) * 3 });

    return std::shared_ptr<Pipeline>(new Pipeline(device, vertex_inputs, shader_program, parameters, render_pass));
}

Pipeline::~Pipeline() {
    _device->delete_pipeline(_pipeline);
}

void Pipeline::bind(backend::Handle<backend::CommandList> const& command_list) {
    _device->bind_pipeline(command_list, _pipeline);
}

std::shared_ptr<ShaderProgram> Pipeline::shader_program() const {
    return _shader_program;
}

backend::FillMode constexpr Pipeline::get_fill_mode(asset::MaterialPassFillMode const fill_mode) const {
    switch(fill_mode) {
        case asset::MaterialPassFillMode::Solid: return backend::FillMode::Solid;
        case asset::MaterialPassFillMode::Wireframe: return backend::FillMode::Wireframe;
        default: return backend::FillMode::Solid;
    }
}

backend::CullMode constexpr Pipeline::get_cull_mode(asset::MaterialPassCullMode const cull_mode) const {
    switch(cull_mode) {
        case asset::MaterialPassCullMode::Back: return backend::CullMode::Back;
        case asset::MaterialPassCullMode::Front: return backend::CullMode::Front;
        case asset::MaterialPassCullMode::None: return backend::CullMode::None;
        default: return backend::CullMode::None; 
    }
}
