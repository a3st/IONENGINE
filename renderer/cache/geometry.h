// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/context.h>
#include <asset/mesh.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

template<class Type>
struct CacheEntry {
    Type value;
    uint64_t value_hash;
};

class GeometryBuffer {
public:

    GeometryBuffer(Context& context, asset::VertexBuffer const& vertex_buffer, asset::IndexBuffer const& index_buffer);

    ~GeometryBuffer();

private:

    Context* _context;

    backend::Handle<backend::Buffer> _vertex_buffer;
    backend::Handle<backend::Buffer> _index_buffer;
};

class GeometryCache {
public:

    GeometryCache();

    GeometryBuffer& get_from(Context& context, asset::Mesh& mesh);

private:

    std::vector<CacheEntry<GeometryBuffer>> _cache;

};

}