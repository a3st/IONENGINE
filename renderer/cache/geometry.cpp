// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/cache/geometry.h>

using namespace ionengine;
using namespace ionengine::renderer;

GeometryCache::GeometryCache() {

}

frontend::GeometryBuffer& GeometryCache::get(frontend::Context& context, asset::MeshSurface& surface) {

    uint64_t const hash = surface.vertices.hash() ^ surface.indices.hash();

    if(_buffers.is_valid(surface.cache_entry)) {

        auto& buffer = _buffers.get(surface.cache_entry);

        if(buffer.value_hash != hash) {

            buffer.value_hash = hash;
        } else {

            return buffer.value;
        }

    } else {

        CacheEntry<frontend::GeometryBuffer> entry = {
            .value = frontend::GeometryBuffer(context, surface),
            .value_hash = hash
        };

        surface.cache_entry = _buffers.push(std::move(entry));
    }
}

void GeometryCache::clear() {

    _buffers.clear();
}
