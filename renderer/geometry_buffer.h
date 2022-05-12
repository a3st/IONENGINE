// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/upload_context.h>
#include <asset/mesh.h>
#include <lib/expected.h>

namespace ionengine::renderer {

class GeometryBuffer {
public:

    GeometryBuffer(backend::Device& device, asset::Surface const& surface, UploadContext& upload_context);

    GeometryBuffer(GeometryBuffer const&) = delete;

    GeometryBuffer(GeometryBuffer&& other) noexcept;

    GeometryBuffer& operator=(GeometryBuffer const&) = delete;

    GeometryBuffer& operator=(GeometryBuffer&& other) noexcept;

    ~GeometryBuffer();

    void bind(backend::Handle<backend::CommandList> const& command_list);

private:

    backend::Device* _device;
    
    backend::Handle<backend::Buffer> _vertex_buffer;
    backend::Handle<backend::Buffer> _index_buffer;

    uint32_t _indices_count{0};
    uint32_t _vertices_count{0};
};

}
