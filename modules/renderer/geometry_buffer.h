// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <renderer/command_list.h>
#include <asset/mesh.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class GeometryBufferError {
    IndexSize,
    IndexOverflow
};

struct GeometryBuffer {
    backend::Handle<backend::Buffer> vertex_buffer;
    backend::Handle<backend::Buffer> index_buffer;
    size_t vertex_size;
    size_t index_size;
    backend::BufferFlags flags;

    bool is_host_visible() const {
        return flags & backend::BufferFlags::HostWrite;
    }

    bool is_vertex_buffer() const {
        return flags & backend::BufferFlags::VertexBuffer;
    }

    bool is_index_buffer() const {
        return flags & backend::BufferFlags::IndexBuffer;
    }

    void bind(backend::Device& device, CommandList& command_list);

    static lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>> create(backend::Device& device, size_t const vertex_size, size_t const index_size, backend::BufferFlags const flags = backend::BufferFlags::VertexBuffer | backend::BufferFlags::IndexBuffer);

    static lib::Expected<GeometryBuffer, lib::Result<GeometryBufferError>> load_from_surface(backend::Device& device, asset::SurfaceData const& surface);
};

template<>
struct GPUResourceDeleter<GeometryBuffer> {
    void operator()(backend::Device& device, GeometryBuffer const& geometry_buffer) const {
        device.delete_buffer(geometry_buffer.vertex_buffer);
        device.delete_buffer(geometry_buffer.index_buffer);
    }
};

}
