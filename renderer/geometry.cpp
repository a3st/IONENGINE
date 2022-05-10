// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/cache/geometry.h>

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::cache;

GeometryCache::GeometryCache() {

}

void GeometryCache::upload(backend::Device& device, asset::Surface const& surface) {


}

frontend::GeometryBuffer& GeometryCache::get(backend::Device& device, asset::Surface& surface) {

    uint64_t const hash = surface.vertices.hash() ^ surface.indices.hash();

    if(_buffers.is_valid(surface.cache_entry)) {

        auto& buffer = _buffers.get(surface.cache_entry);

        if(buffer.hash != hash) {

            buffer.hash = hash;
        }

        return buffer.value;

    } else {

        CacheEntry<frontend::GeometryBuffer> entry = {
            .value = frontend::GeometryBuffer(device, surface),
            .hash = hash
        };

        size_t result = _buffers.push(std::move(entry));
        return _buffers.get(result).value;
    }
}

void GeometryCache::clear() {

    _buffers.clear();
}
