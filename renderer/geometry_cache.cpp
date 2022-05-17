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

std::shared_ptr<GeometryBuffer> GeometryCache::get(UploadContext& context, asset::Mesh& mesh, uint32_t const surface_index) {
        
    uint64_t const total_hash = mesh.surfaces()[surface_index].vertices.hash() ^ mesh.surfaces()[surface_index].indices.hash();

    if(_data.is_valid(mesh.surfaces()[surface_index].cache_entry)) {
        auto& cache_entry = _data.get(mesh.surfaces()[surface_index].cache_entry);
            
        if(cache_entry.hash != mesh.surfaces()[surface_index].cache_entry) {

        }

        return cache_entry.value;

    } else {

        {
            auto buffer = GeometryBuffer::from_surface(*_device, context, mesh.surfaces()[surface_index]);

            auto cache_entry = CacheEntry<std::shared_ptr<GeometryBuffer>> {
                .value = buffer,
                .hash = total_hash
            };

            mesh.surfaces()[surface_index].cache_entry = _data.push(std::move(cache_entry));
        }
        auto& cache_entry = _data.get(mesh.surfaces()[surface_index].cache_entry);
        return cache_entry.value;
    }
}
