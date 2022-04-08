
#include <precompiled.h>
#include <renderer/cache/vertex.h>

using namespace ionengine::renderer;

VertexBuffer::VertexBuffer(backend::Backend& backend, asset::VertexBuffer const& vertex_buffer, asset::IndexBuffer const& index_buffer) 
    : _backend(&backend) {

    _vertex_buffer = backend.create_buffer(65536, backend::BufferFlags::VertexBuffer);

    _index_buffer = backend.create_buffer(65536, backend::BufferFlags::IndexBuffer);
}

VertexBuffer::~VertexBuffer() {

    _backend->delete_buffer(_vertex_buffer);
    _backend->delete_buffer(_index_buffer);
}

VertexCache::VertexCache() {

}

VertexBuffer& VertexCache::get_from(backend::Backend& backend, asset::Mesh& mesh) {

    uint64_t hash = mesh.vertex_buffer().data_hash() ^ mesh.index_buffer().data_hash();

    auto it = std::find_if(_cache.begin(), _cache.end(),
        [&](auto& other) {
            return hash == other.value_hash;
        }
    );

    if(it != _cache.end()) {


        return it->value;

    } else {

        _cache.emplace_back(
            VertexBuffer(backend, mesh.vertex_buffer(), mesh.index_buffer()),
            hash
        );

        return _cache.back().value;
    }
}