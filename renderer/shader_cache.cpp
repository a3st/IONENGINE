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

    uint64_t const hash = technique.hash;

    if(_data.is_valid(technique.cache_entry)) {

        auto& cache_entry = _data.get(technique.cache_entry);   
        return cache_entry.value;

    } else {

        auto result = ShaderProgram::load_from_technique(*_device, technique);
        if(result.is_ok()) {

            auto cache_entry = CacheEntry<ResourcePtr<ShaderProgram>> {
                .value = std::move(result.value()),
                .hash = hash
            };

            auto resource = std::move(cache_entry.value.commit_pending());
            cache_entry.value.commit_ok(std::move(resource));

            technique.cache_entry = _data.push(std::move(cache_entry));
        } else {
            throw lib::Exception(result.error_value().message);
        }

        auto& cache_entry = _data.get(technique.cache_entry);
        return cache_entry.value;
    }
}

void ShaderCache::update(float const delta_time) {

    // for(auto& cache_entry : _data.has_values()) { }
}
