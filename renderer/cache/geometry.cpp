// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/cache/geometry.h>

using namespace ionengine;
using namespace ionengine::renderer;

GeometryCache::GeometryCache() {

}

frontend::GeometryBuffer& GeometryCache::get(frontend::Context& context, asset::MeshSurface& surface) {

    /*uint64_t hash = mesh.vertex_buffer().data_hash() ^ mesh.index_buffer().data_hash();

    auto it = std::find_if(_cache.begin(), _cache.end(),
        [&](auto& other) {
            return hash == other.value_hash;
        }
    );

    if(it != _cache.end()) {
        return it->value;
    } else {
        _cache.emplace_back(GeometryBuffer(context, mesh.vertex_buffer(), mesh.index_buffer()), hash);
        return _cache.back().value;
    }*/
}

void GeometryCache::clear() {

    _buffers.clear();
}
