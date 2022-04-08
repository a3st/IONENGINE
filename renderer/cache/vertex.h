#pragma once

#include <renderer/backend.h>
#include <asset/mesh.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

template<class Type>
struct CacheEntry {
    Type value;
    uint64_t value_hash;
};

class VertexBuffer {
public:

    VertexBuffer(backend::Backend& backend, asset::VertexBuffer const& vertex_buffer, asset::IndexBuffer const& index_buffer);

    ~VertexBuffer();

private:

    backend::Backend* _backend;

    Handle<backend::Buffer> _vertex_buffer;
    Handle<backend::Buffer> _index_buffer;
};

class VertexCache {
public:

    VertexCache();

    VertexBuffer& get_from(backend::Backend& backend, asset::Mesh& mesh);

private:

    std::vector<CacheEntry<VertexBuffer>> _cache;

};

}