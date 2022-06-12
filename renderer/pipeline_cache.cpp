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

ResourcePtr<GPUPipeline> PipelineCache::get(ResourcePtr<Shader> shader, ResourcePtr<RenderPass> render_pass) {
    uint64_t hash;
    std::unordered_map<uint64_t, CacheEntry<ResourcePtr<GPUPipeline>>>::iterator it;
    {
        std::shared_lock lock(_mutex);
        hash = shader->get().hash ^ render_pass->get().hash;

        it = _data.find(hash);

        if(it != _data.end()) {
            return it->second.value;
        }
    }

    std::lock_guard lock(_mutex);

    auto result = GPUPipeline::create_from_shader(*_device, shader->get(), render_pass->get());

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
