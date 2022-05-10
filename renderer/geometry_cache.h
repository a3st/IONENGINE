// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/cache_ptr.h>
#include <renderer/geometry_buffer.h>
#include <renderer/upload_context.h>
#include <asset/mesh.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

template<>
class CachePool<GeometryBuffer> {
public:

    CachePool(backend::Device& device, UploadContext& upload_context) :
        _device(&device),
        _upload_context(&upload_context) {
        
    }

    CachePtr<GeometryBuffer> get(asset::Surface& surface) {

        uint64_t const hash = surface.vertices.hash();

        if(_data.is_valid(surface.cache_entry)) {
            auto& cache_entry = _data.get(surface.cache_entry);
            
            if(cache_entry.hash != surface.cache_entry) {


            }

            return cache_entry.value;

        } else {

            auto cache_entry = CacheEntry<CachePtr<GeometryBuffer>> {
                .value = CachePtr<GeometryBuffer>(GeometryBuffer(*_device, surface)),
                .hash = hash
            };

            _upload_context->upload(
                cache_entry.value, 
                std::span<uint8_t const>((uint8_t*)surface.vertices.data(), surface.vertices.size()), 
                std::span<uint8_t const>((uint8_t*)surface.indices.data(), surface.indices.size())
            );

            surface.cache_entry = _data.push(std::move(cache_entry));
            return _data.get(surface.cache_entry).value;
        }
    }

private:

    backend::Device* _device;
    UploadContext* _upload_context;

    lib::SparseVector<CacheEntry<CachePtr<GeometryBuffer>>> _data;
};

}
