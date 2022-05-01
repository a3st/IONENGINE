// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <renderer/frontend/geometry_buffer.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

template<class Type>
struct CacheEntry {
    Type value;
    uint64_t value_hash;
};

class GeometryCache {
public:

    GeometryCache();

    frontend::GeometryBuffer& get(frontend::Context& context, asset::MeshSurface& surface);

    void clear();

private:

    lib::SparseVector<CacheEntry<frontend::GeometryBuffer>> _buffers;

};

}
