// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/geometry.h>

using namespace ionengine::renderer;

GeometryBuffer::GeometryBuffer(Context& context, asset::VertexBuffer const& vertex_buffer, asset::IndexBuffer const& index_buffer) 
    : _context(&context) {

    _vertex_buffer = _context->device().create_buffer(65536, backend::BufferFlags::VertexBuffer | backend::BufferFlags::HostWrite);
    _index_buffer = _context->device().create_buffer(65536, backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite);

    context.device().upload_buffer_data(_vertex_buffer, 0, vertex_buffer.data());
    context.device().upload_buffer_data(_index_buffer, 0, index_buffer.data());

    // UploadBuffer& buffer = context.get_or_allocate_upload_buffer(UploadBufferFlags::Sync);
    // buffer.upload_data(context.device(), _vertex_buffer, 0, vertex_buffer.data());
    // buffer.upload_data(context.device(), _index_buffer, 0, index_buffer.data());
    // buffer

    // context.device().is_completed(QueueFlags::Copy, fence_value) == true / false
    //
}

GeometryBuffer::~GeometryBuffer() {

    _context->device().delete_buffer(_vertex_buffer);
    _context->device().delete_buffer(_index_buffer);
}