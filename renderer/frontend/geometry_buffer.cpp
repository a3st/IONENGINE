// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/geometry_buffer.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

GeometryBuffer::GeometryBuffer(Context& context, asset::Surface const& surface, BufferUsage const usage) :
    _context(&context) {

    _vertex_buffer = _context->device().create_buffer(65536, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);
    _index_buffer = _context->device().create_buffer(65536, backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite);
    
    auto& primitive = surface.primitives[0];

    _context->device().map_buffer_data(_vertex_buffer, 0, std::span<uint8_t const>(primitive.vertex_buffer.data(), primitive.vertex_buffer.size()));

    _vertex_count = primitive.vertex_buffer.count();
}

GeometryBuffer::GeometryBuffer(Context& context) :
    _context(&context) {

}

GeometryBuffer::GeometryBuffer(GeometryBuffer&& other) noexcept {

    _context = other._context;
    _vertex_buffer = other._vertex_buffer;
    _index_buffer = (other._index_buffer);
    _vertex_inputs = other._vertex_inputs;
    _vertex_count = other._vertex_count;

    other._vertex_buffer = backend::InvalidHandle<backend::Buffer>();
    other._index_buffer.reset();
}

GeometryBuffer& GeometryBuffer::operator=(GeometryBuffer&& other) noexcept {

    _context = other._context;
    _vertex_buffer = other._vertex_buffer;
    _index_buffer = other._index_buffer;
    _vertex_inputs = other._vertex_inputs;
    _vertex_count = other._vertex_count;

    other._vertex_buffer = backend::InvalidHandle<backend::Buffer>();
    other._index_buffer.reset();
    return *this;
}

GeometryBuffer::~GeometryBuffer() {

    if(_vertex_buffer != backend::InvalidHandle<backend::Buffer>()) {
        _context->device().delete_buffer(_vertex_buffer);
    }

    if(_index_buffer.has_value()) {
        _context->device().delete_buffer(_index_buffer.value());
    }
}

void GeometryBuffer::bind(backend::Handle<backend::CommandList> const& command_list) {

    _context->device().bind_vertex_buffer(command_list, 0, _vertex_buffer, 0);
    _context->device().draw(command_list, _vertex_count, 1, 0);
}

GeometryBuffer GeometryBuffer::quad(Context& context) {

    auto quad_vertices = std::vector<float> {
        1.0f, 1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, -1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, -1.0f
    };

    auto vertex_declaration = std::vector<backend::VertexInputDesc> {
        backend::VertexInputDesc { "POSITION", 0, backend::Format::RG32, 0, 0 },
        backend::VertexInputDesc { "TEXCOORD", 0, backend::Format::RG32, 0, sizeof(float) * 2 }
    };

    GeometryBuffer geometry_buffer(context);
    geometry_buffer._vertex_buffer = context.device().create_buffer(100, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);
    
    context.device().map_buffer_data(
        geometry_buffer._vertex_buffer, 
        0, 
        std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(quad_vertices.data()), quad_vertices.size() * sizeof(float))
    );

    geometry_buffer._vertex_count = 6;
    geometry_buffer._vertex_inputs = std::move(vertex_declaration);
    return geometry_buffer;
}

std::span<backend::VertexInputDesc const> GeometryBuffer::vertex_declaration() const {

    return _vertex_inputs;
}
