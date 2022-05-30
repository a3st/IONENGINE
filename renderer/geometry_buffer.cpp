// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>> GeometryBuffer::procedural(backend::Device& device, uint32_t const vertices_size, uint32_t const indices_size) {

    auto geometry_buffer = GeometryBuffer {};
    {
        size_t const vertices_size_bytes = vertices_size * sizeof(float);
        geometry_buffer.vertex_buffer = device.create_buffer(vertices_size_bytes, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);

        size_t const indices_size_bytes = indices_size * sizeof(uint32_t);
        geometry_buffer.index_buffer = device.create_buffer(indices_size_bytes, backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite);

        geometry_buffer.flags = backend::BufferFlags::VertexBuffer | backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite;

        geometry_buffer.vertices_size = vertices_size;
        geometry_buffer.indices_size = indices_size;
    }
    return lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>>::ok(std::move(geometry_buffer));
}

lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>> GeometryBuffer::load_from_surface(backend::Device& device, asset::SurfaceData const& surface) {

    if(surface.indices.size() / sizeof(uint32_t) > std::numeric_limits<uint16_t>::max()) {
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

        geometry_buffer.vertices_size = static_cast<uint32_t>(surface.vertices.size());
        geometry_buffer.indices_size = static_cast<uint32_t>(surface.indices.size());
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
