// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GPUBuffer, lib::Result<GPUBufferError>> GPUBuffer::cbuffer(backend::Device& device, uint32_t const size, backend::BufferFlags const flags) {

    auto gpu_buffer = GPUBuffer {};
    {
        gpu_buffer.buffer = device.create_buffer(size, flags);
        gpu_buffer.flags = flags;
        gpu_buffer.size = size;
        if(flags & backend::BufferFlags::HostWrite) {
            gpu_buffer.memory_state = backend::MemoryState::GenericRead;
        } else {
            gpu_buffer.memory_state = backend::MemoryState::Common;
        }
    }
    return lib::Expected<GPUBuffer, lib::Result<GPUBufferError>>::ok(std::move(gpu_buffer));
}

lib::Expected<GPUBuffer, lib::Result<GPUBufferError>> GPUBuffer::sbuffer(backend::Device& device, uint32_t const size, backend::BufferFlags const flags, uint32_t const element_stride) {

    auto gpu_buffer = GPUBuffer {};
    {
        gpu_buffer.buffer = device.create_buffer(size, flags, element_stride);
        gpu_buffer.flags = flags;
        gpu_buffer.size = size;
        if(flags & backend::BufferFlags::HostWrite) {
            gpu_buffer.memory_state = backend::MemoryState::GenericRead;
        } else {
            gpu_buffer.memory_state = backend::MemoryState::Common;
        }
    }
    return lib::Expected<GPUBuffer, lib::Result<GPUBufferError>>::ok(std::move(gpu_buffer));
}
