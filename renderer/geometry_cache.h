// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/upload_manager.h>
#include <asset/mesh.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

class GeometryCache {
public:

    GeometryCache(backend::Device& device);

    GeometryCache(GeometryCache const&) = delete;

    GeometryCache(GeometryCache&& other) noexcept;

    GeometryCache& operator=(GeometryCache const&) = delete;

    GeometryCache& operator=(GeometryCache&& other) noexcept;

    ResourcePtr<GeometryBuffer> get(UploadManager& upload_manager, asset::Mesh& mesh, uint32_t const index);

private:

    backend::Device* _device;

    lib::SparseVector<CacheEntry<ResourcePtr<GeometryBuffer>>> _data;
};

}
