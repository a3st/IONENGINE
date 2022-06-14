// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/pipeline_cache.h>

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

uint64_t ionengine::renderer::gpu_pipeline_calculate_hash(GPUProgram const& program, asset::ShaderDrawParameters const& draw_parameters, RenderPass const& render_pass) {

    uint32_t stage_hash = program.stages.at(0).index() ^ program.stages.at(1).generation();

    for(auto const& stage : program.stages) {
        stage_hash ^= stage.index() ^ stage.generation();
    }

    uint32_t const draw_parameters_hash = 
        static_cast<uint32_t>(draw_parameters.fill_mode) ^
        static_cast<uint32_t>(draw_parameters.cull_mode) ^
        static_cast<uint32_t>(draw_parameters.depth_stencil) ^
        static_cast<uint32_t>(draw_parameters.blend)
    ;

    uint32_t const render_pass_hash = render_pass.render_pass.index() ^ render_pass.render_pass.generation();

    uint32_t const high = stage_hash ^ draw_parameters_hash;
    uint32_t const low = render_pass_hash;

    return ((static_cast<uint64_t>(high) << 32) | static_cast<uint64_t>(low));
}

ResourcePtr<GPUPipeline> PipelineCache::get(GPUProgram const& program, asset::ShaderDrawParameters const& draw_parameters, RenderPass const& render_pass) {
    uint64_t hash;
    {
        std::shared_lock lock(_mutex);
        hash = gpu_pipeline_calculate_hash(program, draw_parameters, render_pass);

        auto it = _data.find(hash);

        if(it != _data.end()) {
            return it->second.value;
        }
    }

    std::lock_guard lock(_mutex);

    auto result = GPUPipeline::create(*_device, program, draw_parameters, render_pass);

    if(result.is_ok()) {

        GPUPipeline gpu_pipeline = std::move(result.as_ok());

        auto cache_entry = CacheEntry<ResourcePtr<GPUPipeline>> {
            .value = make_resource_ptr(std::move(gpu_pipeline)),
            .hash = hash
        };
        
        return _data.insert({ hash, std::move(cache_entry) }).first->second.value;
    } else {
        throw lib::Exception(result.as_error().message);
    }
}
