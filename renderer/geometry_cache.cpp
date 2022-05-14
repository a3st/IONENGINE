// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry_cache.h>

using namespace ionengine;
using namespace ionengine::renderer;

GeometryCache::GeometryCache(backend::Device& device) :
    _device(&device) {
}

GeometryCache::GeometryCache(GeometryCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
}

GeometryCache& GeometryCache::operator=(GeometryCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
    return *this;
}

std::shared_ptr<GeometryBuffer> GeometryCache::get(UploadContext& context, asset::Surface& surface) {
        
    uint64_t const total_hash = surface.hash();

    if(_data.is_valid(surface.cache_entry())) {
        auto& cache_entry = _data.get(surface.cache_entry());
            
        if(cache_entry.hash != surface.cache_entry()) {

        }

        return cache_entry.value;

    } else {

        {
            auto buffer = GeometryBuffer::from_surface(*_device, context, surface);

            auto cache_entry = CacheEntry<std::shared_ptr<GeometryBuffer>> {
                .value = buffer,
                .hash = total_hash
            };

            surface.cache_entry(_data.push(std::move(cache_entry)));
        }
        auto& cache_entry = _data.get(surface.cache_entry());
        return cache_entry.value;
    }
}
