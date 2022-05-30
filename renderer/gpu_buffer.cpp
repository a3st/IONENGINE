// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_buffer.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GPUBuffer, lib::Result<GPUBufferError>> GPUBuffer::create(backend::Device& device, size_t const size, backend::BufferFlags const flags, uint32_t const element_stride) {

    if(flags & backend::BufferFlags::ConstantBuffer && size != ((size + (backend::TEXTURE_ROW_PITCH_ALIGNMENT - 1)) & ~(backend::TEXTURE_ROW_PITCH_ALIGNMENT - 1))) {
        return lib::Expected<GPUBuffer, lib::Result<GPUBufferError>>::error(
            lib::Result<GPUBufferError> { .errc = GPUBufferError::ConstantAlignment, .message = "constant buffer not 256 byte aligned" }
        );
    }

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
