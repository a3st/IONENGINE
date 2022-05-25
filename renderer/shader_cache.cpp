// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_cache.h>

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

ResourcePtr<ShaderProgram> ShaderCache::get(asset::Technique& technique) {

    if(_data.is_valid(technique.cache_entry)) {
        auto& cache_entry = _data.get(technique.cache_entry);
            
        if(cache_entry.hash != technique.cache_entry) {

        }

        return cache_entry.value;

    } else {

        {
            auto program = ShaderProgram::from_technique(*_device, technique);

            auto cache_entry = CacheEntry<std::shared_ptr<ShaderProgram>> {
                .value = program,
                .hash = technique.hash
            };

            technique.cache_entry = _data.push(std::move(cache_entry));
        }
        auto& cache_entry = _data.get(technique.cache_entry);
        return cache_entry.value;
    }
}

void ShaderCache::update(float const delta_time) {

    // for(auto& cache_entry : _data.has_values()) { }
}