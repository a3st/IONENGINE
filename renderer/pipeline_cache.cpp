// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/pipeline_cache.h>
#include <lib/hash/crc32.h>

using namespace ionengine;
using namespace ionengine::renderer;

PipelineCache::PipelineCache(backend::Device& device) :
    _device(&device) {
}

std::shared_ptr<Pipeline> PipelineCache::get(
    ShaderCache& shader_cache,
    asset::Material& material, 
    std::string_view const render_pass_name, 
    backend::Handle<backend::RenderPass> const& render_pass
) {

    std::string const pipeline_name = std::format("{}_{}", render_pass_name, render_pass.index());
    uint32_t const pipeline_hash = lib::hash::ctcrc32(pipeline_name.data(), pipeline_name.size());

    if(_data.find(pipeline_hash) == _data.end()) {
        
        for(auto const& pass : material.passes()) {
            if(pass.name == render_pass_name) {
                auto shader_program = shader_cache.get(*pass.technique);
            
                auto pipeline = Pipeline::from_data(
                    *_device,
                    shader_program,
                    pass.parameters,
                    render_pass
                );

                _data.insert({ pipeline_hash, pipeline });
                break;
            }
            continue;
        }
    }

    return _data.at(pipeline_hash);
}
