// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/pipeline_cache.h>
#include <lib/hash/crc32.h>

using namespace ionengine;
using namespace ionengine::renderer;

PipelineCache::PipelineCache(backend::Device& device) :
    _device(&device) {
}

PipelineCache::PipelineCache(PipelineCache&& other) noexcept {
    _device = std::move(_device);
    _data = std::move(_data);
}

PipelineCache& PipelineCache::operator=(PipelineCache&& other) noexcept {
    _device = std::move(_device);
    _data = std::move(_data);
    return *this;
}

std::tuple<backend::Handle<backend::Pipeline>, ResourcePtr<ShaderProgram>> PipelineCache::get(
    ShaderCache& shader_cache,
    asset::Technique& technique,
    asset::MaterialPassParameters const& parameters,
    backend::Handle<backend::RenderPass> const& render_pass
) {

    std::string const pipeline_name = std::format("{}_{}", technique.name, render_pass.index());
    uint32_t const pipeline_hash = lib::hash::ctcrc32(pipeline_name.data(), pipeline_name.size());

    auto shader_program = shader_cache.get(technique);

    if(_data.find(pipeline_hash) == _data.end()) {

        backend::RasterizerDesc rasterizer_desc = {
            .fill_mode = get_fill_mode(parameters.fill_mode),
            .cull_mode = get_cull_mode(parameters.cull_mode)
        };

        backend::DepthStencilDesc depth_stencil_desc = { backend::CompareOp::Less, parameters.depth_stencil };

        backend::BlendDesc blend_desc = { 
            false, 
            backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, 
            backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add 
        };

        auto pipeline = _device->create_pipeline(
            shader_program->descriptor_layout, 
            shader_program->attributes, 
            shader_program->shaders, 
            rasterizer_desc, 
            depth_stencil_desc, 
            blend_desc, 
            render_pass, 
            backend::InvalidHandle<backend::CachePipeline>()
        );

        _data.insert({ pipeline_hash, pipeline });
    }

    return { _data.at(pipeline_hash), shader_program };
}

std::tuple<backend::Handle<backend::Pipeline>, ResourcePtr<ShaderProgram>> PipelineCache::get(
    ShaderCache& shader_cache,
    asset::Technique& technique
) {
    std::string const pipeline_name = std::format("{}", technique.name);
    uint32_t const pipeline_hash = lib::hash::ctcrc32(pipeline_name.data(), pipeline_name.size());

    auto shader_program = shader_cache.get(technique);

    if(_data.find(pipeline_hash) == _data.end()) {
        auto pipeline = _device->create_pipeline(
            shader_program->descriptor_layout,
            shader_program->shaders.at(0), 
            backend::InvalidHandle<backend::CachePipeline>()
        );

        _data.insert({ pipeline_hash, pipeline });
    }

    return { _data.at(pipeline_hash), shader_program };
}

backend::FillMode constexpr ionengine::renderer::get_fill_mode(asset::MaterialPassFillMode const fill_mode) {
    switch(fill_mode) {
        case asset::MaterialPassFillMode::Solid: return backend::FillMode::Solid;
        case asset::MaterialPassFillMode::Wireframe: return backend::FillMode::Wireframe;
        default: {
            assert(false && "invalid data type");
            return backend::FillMode::Solid; 
        }
    }
}

backend::CullMode constexpr ionengine::renderer::get_cull_mode(asset::MaterialPassCullMode const cull_mode) {
    switch(cull_mode) {
        case asset::MaterialPassCullMode::Back: return backend::CullMode::Back;
        case asset::MaterialPassCullMode::Front: return backend::CullMode::Front;
        case asset::MaterialPassCullMode::None: return backend::CullMode::None;
        default: {
            assert(false && "invalid data type");
            return backend::CullMode::None; 
        }
    }
}
