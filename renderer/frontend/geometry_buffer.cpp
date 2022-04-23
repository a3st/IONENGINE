// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/frontend/geometry_buffer.h>

using namespace ionengine::renderer;
using namespace ionengine::renderer::frontend;

GeometryBuffer::GeometryBuffer(Context& context, asset::Model const& model, BufferUsage const usage) :
    _context(&context) {

    _buffer = _context->device().create_buffer(65536, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);

}

GeometryBuffer::~GeometryBuffer() {

    
}

void GeometryBuffer::bind(backend::Handle<backend::CommandList> const& command_list) {

    _context->device().draw_indexed(command_list, 1, 1, 0);
}