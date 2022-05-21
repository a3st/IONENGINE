// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

GeometryBuffer::GeometryBuffer(backend::Device& device, uint32_t const vertices_size, uint32_t const indices_size) :
    _device(&device), _vertices_size(vertices_size), _indices_size(indices_size) {

    size_t const vertices_size_bytes = vertices_size * sizeof(float);
    _vertex_buffer = device.create_buffer(vertices_size_bytes, backend::BufferFlags::VertexBuffer);

    size_t const indices_size_bytes = indices_size * sizeof(uint32_t);
    _index_buffer = device.create_buffer(indices_size_bytes, backend::BufferFlags::IndexBuffer);
}

std::shared_ptr<GeometryBuffer> GeometryBuffer::from_surface(backend::Device& device, UploadContext& upload_context, asset::SurfaceData const& surface) {
    auto buffer = std::shared_ptr<GeometryBuffer>(new GeometryBuffer(device, static_cast<uint32_t>(surface.vertices.size()), static_cast<uint32_t>(surface.indices.size())));
    buffer->copy_vertex_data(upload_context, std::span<float const>(surface.vertices.data(), surface.vertices.size()));
    buffer->copy_index_data(upload_context, std::span<uint32_t const>(surface.indices.data(), surface.indices.size()));
    return buffer;
}

void GeometryBuffer::copy_vertex_data(UploadContext& context, std::span<float const> const data) {
    context.begin();
    context.copy_buffer_data(_vertex_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(data.data()), data.size() * sizeof(float)));
    context.end();
}

void GeometryBuffer::copy_index_data(UploadContext& context, std::span<uint32_t const> const data) {
    context.begin();
    context.copy_buffer_data(_index_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(data.data()), data.size() * sizeof(uint32_t)));
    context.end();
}

GeometryBuffer::~GeometryBuffer() {
    _device->delete_buffer(_vertex_buffer);
    _device->delete_buffer(_index_buffer);
}

void GeometryBuffer::bind(backend::Handle<backend::CommandList> const& command_list) {
    _device->bind_vertex_buffer(command_list, 0, _vertex_buffer, 0);
    _device->bind_index_buffer(command_list, _index_buffer, 0);
}

void GeometryBuffer::draw(backend::Handle<backend::CommandList> const& command_list, uint32_t const instance_count) {
    _device->draw_indexed(command_list, _indices_size, instance_count, 0);
}
