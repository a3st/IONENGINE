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

backend::Handle<backend::Pipeline> PipelineCache::get(Shader& shader, backend::Handle<backend::RenderPass> const& render_pass) {

    std::lock_guard lock(_mutex);

    std::string const pipeline_name = std::format("{}_{}", shader.name, render_pass.index());
    uint32_t const pipeline_hash = lib::hash::ctcrc32(pipeline_name.data(), pipeline_name.size());

    if(_data.find(pipeline_hash) == _data.end()) {

        auto pipeline = _device->create_pipeline(
            shader.descriptor_layout, 
            shader.attributes, 
            shader.stages, 
            shader.rasterizer, 
            shader.depth_stencil, 
            shader.blend, 
            render_pass, 
            backend::InvalidHandle<backend::CachePipeline>()
        );

        _data.insert({ pipeline_hash, pipeline });
    }

    return _data.at(pipeline_hash);
}
