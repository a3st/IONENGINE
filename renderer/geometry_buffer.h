// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <asset/mesh.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class GeometryBufferError {
    VertexSize,
    IndexSize,
    IndexOverflow
};

struct GeometryBuffer {
    backend::Handle<backend::Buffer> vertex_buffer;
    backend::Handle<backend::Buffer> index_buffer;
    uint32_t vertices_size;
    uint32_t indices_size;
    backend::BufferFlags flags;

    bool is_host_visible() const {
        return flags & backend::BufferFlags::HostWrite;
    }

    void bind(backend::Device& device, backend::Handle<backend::CommandList> const& command_list);

    void draw_indexed(backend::Device& device, backend::Handle<backend::CommandList> const& command_list, uint32_t const instance_count);

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
