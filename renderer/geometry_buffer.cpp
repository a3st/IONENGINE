// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

GeometryBuffer::GeometryBuffer(backend::Device& device, asset::Surface const& surface, UploadContext& upload_context) :
    _device(&device) {

    size_t const vertices_size_bytes = surface.vertices.size() * sizeof(float);
    _vertex_buffer = device.create_buffer(vertices_size_bytes, backend::BufferFlags::VertexBuffer);

    size_t const indices_size_bytes = surface.indices.size() * sizeof(uint32_t);
    _index_buffer = device.create_buffer(indices_size_bytes, backend::BufferFlags::IndexBuffer);

    upload_context.begin();
    upload_context.copy_buffer_data(_vertex_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(surface.vertices.data()), vertices_size_bytes));
    upload_context.copy_buffer_data(_index_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(surface.indices.data()), indices_size_bytes));
    upload_context.end();

    _vertices_count = surface.vertices.size();
    _indices_count = surface.indices.size();
}

GeometryBuffer::GeometryBuffer(GeometryBuffer&& other) noexcept {

    _device = other._device;
    _vertex_buffer = std::exchange(other._vertex_buffer, backend::InvalidHandle<backend::Buffer>());
    _index_buffer = std::exchange(other._index_buffer, backend::InvalidHandle<backend::Buffer>());
    _indices_count = other._indices_count;
}

GeometryBuffer& GeometryBuffer::operator=(GeometryBuffer&& other) noexcept {

    _device = other._device;
    _vertex_buffer = std::exchange(other._vertex_buffer, backend::InvalidHandle<backend::Buffer>());
    _index_buffer = std::exchange(other._index_buffer, backend::InvalidHandle<backend::Buffer>());
    _indices_count = other._indices_count;
    return *this;
}

GeometryBuffer::~GeometryBuffer() {

    if(_vertex_buffer != backend::InvalidHandle<backend::Buffer>()) {
        _device->delete_buffer(_vertex_buffer);
    }

    if(_index_buffer != backend::InvalidHandle<backend::Buffer>()) {
        _device->delete_buffer(_index_buffer);
    }
}

void GeometryBuffer::bind(backend::Handle<backend::CommandList> const& command_list) {

    _device->bind_vertex_buffer(command_list, 0, _vertex_buffer, 0);
    _device->bind_index_buffer(command_list, _index_buffer, 0);
    _device->draw_indexed(command_list, _indices_count, 1, 0);
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
