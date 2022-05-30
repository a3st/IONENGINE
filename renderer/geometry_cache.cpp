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

    uint64_t const hash = surface.vertices.hash() ^ surface.indices.hash();

    if(_data.is_valid(surface.cache_entry)) {

        auto& cache_entry = _data.get(surface.cache_entry);

        if(cache_entry.value.is_common()) {
            upload_manager.upload_geometry_data(
                cache_entry.value, 
                std::span<uint8_t const>(surface.vertices.data(), surface.vertices.size()),
                std::span<uint8_t const>(surface.indices.data(), surface.indices.size())
            );
        }

        return cache_entry.value;

    } else {

        auto result = GeometryBuffer::load_from_surface(*_device, surface);
        if(result.is_ok()) {

            auto cache_entry = CacheEntry<ResourcePtr<GeometryBuffer>> {
                .value = std::move(result.value()),
                .hash = hash
            };

            upload_manager.upload_geometry_data(
                cache_entry.value, 
                std::span<uint8_t const>(surface.vertices.data(), surface.vertices.size()),
                std::span<uint8_t const>(surface.indices.data(), surface.indices.size())
            );

            surface.cache_entry = _data.push(std::move(cache_entry));
        } else {
            throw lib::Exception(result.error_value().message);
        }

        auto& cache_entry = _data.get(surface.cache_entry);
        return cache_entry.value;
    }
}
