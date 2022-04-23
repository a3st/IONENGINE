// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/geometry_buffer.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

GeometryBuffer::GeometryBuffer(Context& context, asset::Surface const& surface, BufferUsage const usage) :
    _context(&context) {

    _vertex_buffer = _context->device().create_buffer(65536, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);
    _index_buffer = _context->device().create_buffer(65536, backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite);
    
    auto& primitive = surface.primitives[0];

    _context->device().map_buffer_data(_vertex_buffer, 0, std::span<uint8_t const>(primitive.vertex_buffer.data(), primitive.vertex_buffer.size()));

    _vertex_count = primitive.vertex_buffer.count();
}

GeometryBuffer::~GeometryBuffer() {

    _context->device().delete_buffer(_vertex_buffer);
    _context->device().delete_buffer(_index_buffer);
}

void GeometryBuffer::bind(backend::Handle<backend::CommandList> const& command_list) {

    _context->device().bind_vertex_buffer(command_list, 0, _vertex_buffer, 0);
    _context->device().draw(command_list, _vertex_count, 1, 0);
}
