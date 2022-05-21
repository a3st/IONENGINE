// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/upload_context.h>
#include <asset/mesh.h>
#include <lib/expected.h>

namespace ionengine::renderer {

class GeometryBuffer {
public:

    ~GeometryBuffer();

    static std::shared_ptr<GeometryBuffer> from_surface(backend::Device& device, UploadContext& upload_context, asset::SurfaceData const& surface);

    void copy_vertex_data(UploadContext& context, std::span<float const> const data);

    void copy_index_data(UploadContext& context, std::span<uint32_t const> const data);

    void bind(backend::Handle<backend::CommandList> const& command_list);

    void draw(backend::Handle<backend::CommandList> const& command_list, uint32_t const instance_count);

private:

    GeometryBuffer(backend::Device& device, uint32_t const vertices_size, uint32_t const indices_size);

    backend::Device* _device;
    
    backend::Handle<backend::Buffer> _vertex_buffer;
    backend::Handle<backend::Buffer> _index_buffer;

    uint32_t _vertices_size{0};
    uint32_t _indices_size{0};
};

}
