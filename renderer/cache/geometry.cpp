
#include <precompiled.h>
#include <renderer/cache/geometry.h>

using namespace ionengine::renderer;

GeometryBuffer::GeometryBuffer(Context& context, asset::VertexBuffer const& vertex_buffer, asset::IndexBuffer const& index_buffer) 
    : _context(&context) {

    _vertex_buffer = _context->device().create_buffer(65536, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);
    _index_buffer = _context->device().create_buffer(65536, backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite);

    context.device().upload_buffer_data(_vertex_buffer, 0, vertex_buffer.data());
    context.device().upload_buffer_data(_index_buffer, 0, index_buffer.data());
}

GeometryBuffer::~GeometryBuffer() {

    _context->device().delete_buffer(_vertex_buffer);
    _context->device().delete_buffer(_index_buffer);
}

GeometryCache::GeometryCache() {

}

GeometryBuffer& GeometryCache::get_from(Context& context, asset::Mesh& mesh) {

    uint64_t hash = mesh.vertex_buffer().data_hash() ^ mesh.index_buffer().data_hash();

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
    }
}
