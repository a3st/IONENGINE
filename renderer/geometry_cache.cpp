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

ResourcePtr<GeometryBuffer> GeometryCache::get(UploadManager& upload_manager, asset::Mesh& mesh, uint32_t const index) {

    uint64_t const hash = mesh.surfaces()[index].vertices.hash() ^ mesh.surfaces()[index].indices.hash();

    if(_data.is_valid(mesh.surfaces()[index].cache_entry)) {

        auto& cache_entry = _data.get(mesh.surfaces()[index].cache_entry);
        
        return cache_entry.value;

    } else {

        auto result = GeometryBuffer::load_from_surface(*_device, mesh.surfaces()[index]);
        if(result.is_ok()) {

            auto cache_entry = CacheEntry<ResourcePtr<GeometryBuffer>> {
                .value = std::move(result.value()),
                .hash = hash
            };

            upload_manager.upload_geometry_data(
                cache_entry.value, 
                std::span<uint8_t const>((uint8_t const*)mesh.surfaces()[index].vertices.data(), mesh.surfaces()[index].vertices.size()),
                std::span<uint8_t const>((uint8_t const*)mesh.surfaces()[index].indices.data(), mesh.surfaces()[index].indices.size())
            );

            mesh.surfaces()[index].cache_entry = _data.push(std::move(cache_entry));
        } else {
            throw lib::Exception(result.error_value().message);
        }

        return nullptr;
    }
}
