// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry_cache.h>
#include <lib/exception.h>

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

ResourcePtr<GeometryBuffer> GeometryCache::get(UploadManager& upload_manager, asset::SurfaceData& surface) {
    uint64_t hash;
    {
        std::lock_guard lock(_mutex);
        hash = surface.vertices.hash() ^ surface.indices.hash();

        if(_data.is_valid(surface.cache_entry)) {

            auto& cache_entry = _data.get(surface.cache_entry);

            if(cache_entry.value->is_wait_for_upload.load()) {
                upload_manager.upload_geometry_data(cache_entry.value, surface.vertices.to_span(),surface.indices.to_span());
            }

            return cache_entry.value;
        }
    }

    std::unique_lock lock(_mutex);

    auto result = GeometryBuffer::load_from_surface(*_device, surface);
    
    if(result.is_ok()) {

        GeometryBuffer geometry_buffer = std::move(result.as_ok());

        auto cache_entry = CacheEntry<ResourcePtr<GeometryBuffer>> {
            .value = make_resource_ptr(std::move(geometry_buffer)),
            .hash = hash
        };

        cache_entry.value->is_wait_for_upload.store(true);

        upload_manager.upload_geometry_data(cache_entry.value, surface.vertices.to_span(),surface.indices.to_span());
        surface.cache_entry = _data.push(std::move(cache_entry));

        return _data.get(surface.cache_entry).value;
    } else {
        throw lib::Exception(result.as_error().message);
    }
}
