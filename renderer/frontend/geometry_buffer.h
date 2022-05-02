// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <asset/mesh.h>

namespace ionengine::renderer::frontend {

class GeometryBuffer {
public:

    GeometryBuffer(Context& context, asset::MeshSurface const& surface);

    GeometryBuffer(GeometryBuffer&& other) noexcept;

    GeometryBuffer& operator=(GeometryBuffer&& other) noexcept;

    ~GeometryBuffer();

    void bind(backend::Handle<backend::CommandList> const& command_list);

private:

    Context* _context;
    
    backend::Handle<backend::Buffer> _vertex_buffer;
    backend::Handle<backend::Buffer> _index_buffer;

    uint32_t _indices_count{0};
};

}
