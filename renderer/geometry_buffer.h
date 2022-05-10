// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <asset/mesh.h>

namespace ionengine::renderer {

class GeometryBuffer {
public:

    GeometryBuffer(backend::Device& device, asset::Surface const& surface);

    GeometryBuffer(GeometryBuffer&& other) noexcept;

    GeometryBuffer& operator=(GeometryBuffer&& other) noexcept;

    ~GeometryBuffer();

    void bind(backend::Handle<backend::CommandList> const& command_list);

private:

    backend::Device* _device;
    
    backend::Handle<backend::Buffer> _vertex_buffer;
    backend::Handle<backend::Buffer> _index_buffer;

    uint32_t _indices_count{0};
};

}
