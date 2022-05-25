// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>> GeometryBuffer::load_from_surface(backend::Device& device, asset::SurfaceData const& surface) {

    if(surface.indices.size() % 3 != 0) {
        return lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>>::error(
            lib::Result<GeometryBufferError> { .errc = GeometryBufferError::IndexSize, .message = "Index size error" }
        );
    }

    if(surface.indices.size() > std::numeric_limits<uint16_t>::max()) {
        return lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>>::error(
            lib::Result<GeometryBufferError> { .errc = GeometryBufferError::IndexOverflow, .message = "Index size overflow" }
        );
    }

    auto geometry_buffer = GeometryBuffer {};
    {
        size_t const vertices_size_bytes = surface.vertices.size() * sizeof(float);
        geometry_buffer.vertex_buffer = device.create_buffer(vertices_size_bytes, backend::BufferFlags::VertexBuffer);

        size_t const indices_size_bytes = surface.indices.size() * sizeof(uint32_t);
        geometry_buffer.index_buffer = device.create_buffer(indices_size_bytes, backend::BufferFlags::IndexBuffer);

        geometry_buffer.flags = backend::BufferFlags::VertexBuffer | backend::BufferFlags::IndexBuffer;

        geometry_buffer.vertices_size = surface.vertices.size();
        geometry_buffer.indices_size = surface.indices.size();
    }
    return lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>>::ok(std::move(geometry_buffer));
}

void GeometryBuffer::bind(backend::Device& device, backend::Handle<backend::CommandList> const& command_list) {
    device.bind_vertex_buffer(command_list, 0, vertex_buffer, 0);
    device.bind_index_buffer(command_list, index_buffer, 0);
}

void GeometryBuffer::draw_indexed(backend::Device& device, backend::Handle<backend::CommandList> const& command_list, uint32_t const instance_count) {
    device.draw_indexed(command_list, indices_size, instance_count, 0);
}

/*
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
*/
