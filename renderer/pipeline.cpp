// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/pipeline.h>

using namespace ionengine;
using namespace ionengine::renderer;

Pipeline::Pipeline(
    backend::Device& device,
    backend::Handle<backend::DescriptorLayout> const& descriptor_layout,
    std::span<backend::VertexInputDesc const> const vertex_descs,
    std::span<backend::Handle<backend::Shader> const> const shaders,
    asset::MaterialPassParameters const& parameters,
    backend::Handle<backend::RenderPass> const& render_pass
) : _device(&device) {

    backend::RasterizerDesc rasterizer_desc = {
        .fill_mode = get_fill_mode(parameters.fill_mode),
        .cull_mode = get_cull_mode(parameters.cull_mode)
    };

    backend::DepthStencilDesc depth_stencil_desc = { backend::CompareOp::Always, false };

    backend::BlendDesc blend_desc = { 
        false, 
        backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, 
        backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add 
    };

    _pipeline = device.create_pipeline(
        descriptor_layout, 
        vertex_descs, 
        shaders, 
        rasterizer_desc, 
        depth_stencil_desc, 
        blend_desc, 
        render_pass, 
        backend::InvalidHandle<backend::CachePipeline>()
    );
}

std::shared_ptr<Pipeline> Pipeline::from_data(
    backend::Device& device,
    backend::Handle<backend::DescriptorLayout> const& descriptor_layout,
    std::span<backend::Handle<backend::Shader> const> const shaders,
    asset::MaterialPassParameters const& parameters,
    backend::Handle<backend::RenderPass> const& render_pass
) {
    std::vector<backend::VertexInputDesc> vertex_inputs;
    vertex_inputs.emplace_back("POSITION", 0, backend::Format::RGB32, 0, 0);
    vertex_inputs.emplace_back("COLOR", 0, backend::Format::RGB32, 0, sizeof(float) * 3);

    return std::shared_ptr<Pipeline>(new Pipeline(device, descriptor_layout, vertex_inputs, shaders, parameters, render_pass));
}

Pipeline::~Pipeline() {
    _device->delete_pipeline(_pipeline);
}

void Pipeline::bind(backend::Handle<backend::CommandList> const& command_list) {
    _device->bind_pipeline(command_list, _pipeline);
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
