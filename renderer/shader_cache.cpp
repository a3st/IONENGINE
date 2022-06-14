// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_cache.h>
#include <lib/logger.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderCache::ShaderCache(backend::Device& device) :
    _device(&device) {
}

ShaderCache::ShaderCache(ShaderCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
}

ShaderCache& ShaderCache::operator=(ShaderCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
    return *this;
}

ResourcePtr<GPUProgram> ShaderCache::get(asset::Shader& shader) {
    
    {
        std::shared_lock lock(_mutex);
        uint64_t const hash = shader.hash;

        if(_data.is_valid(shader.cache_entry)) {
            auto& cache_entry = _data.get(shader.cache_entry);
            return cache_entry.value;
        }
    }

    std::unique_lock lock(_mutex);

    auto result = GPUProgram::load_from_shader(*_device, shader);

    if(result.is_ok()) {
        GPUProgram program = std::move(result.as_ok());

        auto cache_entry = CacheEntry<ResourcePtr<GPUProgram>> {
            .value = make_resource_ptr(result.as_ok()),
            .hash = shader.hash
        };

        shader.cache_entry = _data.push(std::move(cache_entry));

        return _data.get(shader.cache_entry).value;
    } else {
        throw lib::Exception(result.as_error().message);
    }
}
