// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

GPUBuffer::GPUBuffer(backend::Device& device, size_t const size, backend::BufferFlags const flags, uint32_t const element_stride) :
    _device(&device) {

    if(flags & backend::BufferFlags::ShaderResource) {
        _buffer = device.create_buffer(size, flags, element_stride);
    } else {
        _buffer = device.create_buffer(size, flags);
    }
    
    _flags = flags;
}

GPUBuffer::~GPUBuffer() {
    _device->delete_buffer(_buffer);
}

std::shared_ptr<GPUBuffer> GPUBuffer::cbuffer(backend::Device& device, uint32_t const size) {
    return std::shared_ptr<GPUBuffer>(new GPUBuffer(device, static_cast<size_t>(size), backend::BufferFlags::HostWrite | backend::BufferFlags::ConstantBuffer, 0));
}

std::shared_ptr<GPUBuffer> GPUBuffer::sbuffer(backend::Device& device, uint32_t const size, uint32_t const element_stride) {
    return std::shared_ptr<GPUBuffer>(new GPUBuffer(device, static_cast<size_t>(size), backend::BufferFlags::HostWrite | backend::BufferFlags::ShaderResource, element_stride));
}

backend::Handle<backend::Buffer> GPUBuffer::as_buffer() const {
    return _buffer;
}

bool GPUBuffer::is_cbuffer() const {
    return _flags & backend::BufferFlags::ConstantBuffer;
}

void GPUBuffer::barrier(backend::Handle<backend::CommandList> const& command_list, backend::MemoryState const memory_state) {
    _device->barrier(command_list, _buffer, _memory_state, memory_state);
    _memory_state = memory_state;
}

backend::MemoryState GPUBuffer::memory_state() const {
    return _memory_state;
}

void GPUBuffer::copy_data(UploadContext& context, std::span<uint8_t const> const data) {
    if(_flags & backend::BufferFlags::HostWrite) {
        _device->map_buffer_data(_buffer, 0, data);
    } else {
        context.begin();
        context.copy_buffer_data(_buffer, 0, data);
        context.end();
    }
}
