// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>> GeometryBuffer::create(backend::Device& device, size_t const vertex_size, size_t const index_size, backend::BufferFlags const flags) {

    if(index_size / sizeof(uint32_t) > std::numeric_limits<uint16_t>::max()) {
        return lib::make_expected<GeometryBuffer, lib::Result<GeometryBufferError>>(
            lib::Result<GeometryBufferError> { .errc = GeometryBufferError::IndexOverflow, .message = "index buffer has overflow" }
        );
    }

    if((index_size / sizeof(uint32_t)) % 3 != 0) {
        return lib::make_expected<GeometryBuffer, lib::Result<GeometryBufferError>>(
            lib::Result<GeometryBufferError> { .errc = GeometryBufferError::IndexSize, .message = "index buffer contains no triangles" }
        );
    }

    auto geometry_buffer = GeometryBuffer {};
    {
        geometry_buffer.vertex_buffer = device.create_buffer(vertex_size, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);
        geometry_buffer.index_buffer = device.create_buffer(index_size, backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite);
        geometry_buffer.flags = flags;
        geometry_buffer.vertex_size = vertex_size;
        geometry_buffer.index_size = index_size;
    }
    return lib::make_expected<GeometryBuffer, lib::Result<GeometryBufferError>>(std::move(geometry_buffer));
}

lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>> GeometryBuffer::load_from_surface(backend::Device& device, asset::SurfaceData const& surface) {

    if(surface.indices.size() / sizeof(uint32_t) > std::numeric_limits<uint16_t>::max()) {
        return lib::make_expected<GeometryBuffer, lib::Result<GeometryBufferError>>(
            lib::Result<GeometryBufferError> { .errc = GeometryBufferError::IndexOverflow, .message = "index buffer has overflow" }
        );
    }

    if((surface.indices.size() / sizeof(uint32_t)) % 3 != 0) {
        return lib::make_expected<GeometryBuffer, lib::Result<GeometryBufferError>>(
            lib::Result<GeometryBufferError> { .errc = GeometryBufferError::IndexSize, .message = "index buffer contains no triangles" }
        );
    }

    auto geometry_buffer = GeometryBuffer {};
    {
        geometry_buffer.vertex_buffer = device.create_buffer(surface.vertices.size(), backend::BufferFlags::VertexBuffer);
        geometry_buffer.index_buffer = device.create_buffer(surface.indices.size(), backend::BufferFlags::IndexBuffer);
        geometry_buffer.flags = backend::BufferFlags::VertexBuffer | backend::BufferFlags::IndexBuffer;
        geometry_buffer.vertex_size = surface.vertices.size();
        geometry_buffer.index_size = surface.indices.size();
    }
    return lib::make_expected<GeometryBuffer, lib::Result<GeometryBufferError>>(std::move(geometry_buffer));
}

void GeometryBuffer::bind(backend::Device& device, CommandList& command_list) {
    device.bind_vertex_buffer(command_list.command_list, 0, vertex_buffer, 0);
    device.bind_index_buffer(command_list.command_list, index_buffer, 0);
}
