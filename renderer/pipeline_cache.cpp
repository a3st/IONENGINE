// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/pipeline_cache.h>
#include <lib/hash/crc32.h>

using namespace ionengine;
using namespace ionengine::renderer;

PipelineCache::PipelineCache(backend::Device& device) :
    _device(&device) {
}

std::tuple<std::shared_ptr<Pipeline>, std::shared_ptr<ShaderProgram>> PipelineCache::get(
    ShaderCache& shader_cache,
    asset::Technique& technique,
    asset::MaterialPassParameters const& parameters,
    backend::Handle<backend::RenderPass> const& render_pass
) {

    std::string const pipeline_name = std::format("{}_{}", technique.name(), render_pass.index());
    uint32_t const pipeline_hash = lib::hash::ctcrc32(pipeline_name.data(), pipeline_name.size());

    auto shader_program = shader_cache.get(technique);

    if(_data.find(pipeline_hash) == _data.end()) {
        auto pipeline = Pipeline::from_data(*_device, shader_program, parameters, render_pass);
        _data.insert({ pipeline_hash, pipeline });
    }

    return { _data.at(pipeline_hash), shader_program };
}

std::tuple<std::shared_ptr<Pipeline>, std::shared_ptr<ShaderProgram>> PipelineCache::get(
    ShaderCache& shader_cache,
    asset::Technique& technique
) {
    std::string const pipeline_name = std::format("{}", technique.name());
    uint32_t const pipeline_hash = lib::hash::ctcrc32(pipeline_name.data(), pipeline_name.size());

    auto shader_program = shader_cache.get(technique);

    if(_data.find(pipeline_hash) == _data.end()) {
        auto pipeline = Pipeline::compute(*_device, shader_program);
        _data.insert({ pipeline_hash, pipeline });
    }

    return { _data.at(pipeline_hash), shader_program };
}
