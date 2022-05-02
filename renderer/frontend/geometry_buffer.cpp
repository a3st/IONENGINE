// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/geometry_buffer.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

GeometryBuffer::GeometryBuffer(Context& context, asset::MeshSurface const& surface) :
    _context(&context) {

    _vertex_buffer = _context->create_vertex_buffer(static_cast<uint32_t>(surface.vertices.size()));
    _index_buffer = _context->create_index_buffer(static_cast<uint32_t>(surface.indices.size()));
}

GeometryBuffer::GeometryBuffer(GeometryBuffer&& other) noexcept {

    _context = other._context;
    _vertex_buffer = std::exchange(other._vertex_buffer, backend::InvalidHandle<backend::Buffer>());
    _index_buffer = std::exchange(other._index_buffer, backend::InvalidHandle<backend::Buffer>());
    _indices_count = other._indices_count;
}

GeometryBuffer& GeometryBuffer::operator=(GeometryBuffer&& other) noexcept {

    _context = other._context;
    _vertex_buffer = std::exchange(other._vertex_buffer, backend::InvalidHandle<backend::Buffer>());
    _index_buffer = std::exchange(other._index_buffer, backend::InvalidHandle<backend::Buffer>());
    _indices_count = other._indices_count;
    return *this;
}

GeometryBuffer::~GeometryBuffer() {

    if(_vertex_buffer != backend::InvalidHandle<backend::Buffer>()) {
        _context->device().delete_buffer(_vertex_buffer);
    }

    if(_index_buffer != backend::InvalidHandle<backend::Buffer>()) {
        _context->device().delete_buffer(_index_buffer);
    }
}

void GeometryBuffer::bind(backend::Handle<backend::CommandList> const& command_list) {

    _context->device().bind_vertex_buffer(command_list, 0, _vertex_buffer, 0);
    _context->device().bind_index_buffer(command_list, _index_buffer, 0);
    _context->device().draw_indexed(command_list, _indices_count, 1, 0);
}

/*
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
*/