// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <asset/model.h>

namespace ionengine::renderer::frontend {

class GeometryBuffer {
public:

    GeometryBuffer(Context& context, asset::Surface const& surface, BufferUsage const usage);

    GeometryBuffer(GeometryBuffer&& other) noexcept;

    GeometryBuffer& operator=(GeometryBuffer&& other) noexcept;

    static GeometryBuffer quad(Context& context);

    ~GeometryBuffer();

    void bind(backend::Handle<backend::CommandList> const& command_list);

    std::span<backend::VertexInputDesc const> vertex_declaration() const;

private:

    GeometryBuffer(Context& context);

    Context* _context;
    
    backend::Handle<backend::Buffer> _vertex_buffer;
    std::optional<backend::Handle<backend::Buffer>> _index_buffer;

    std::vector<backend::VertexInputDesc> _vertex_inputs;

    uint32_t _vertex_count{0};
    uint32_t _index_count{0};
};

}
