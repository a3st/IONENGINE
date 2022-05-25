// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class GPUBufferError { };

struct GPUBuffer {
    backend::Handle<backend::Buffer> buffer;
    size_t size;
    backend::BufferFlags flags;
    backend::MemoryState memory_state;

    bool is_cbuffer() const {
        return flags & backend::BufferFlags::ConstantBuffer;
    }

    bool is_sbuffer() const {
        return flags & backend::BufferFlags::ShaderResource;
    }

    bool is_rwbuffer() const {
        return flags & backend::BufferFlags::UnorderedAccess;
    }

    bool is_host_visible() const {
        return flags & backend::BufferFlags::HostWrite;
    }

    static lib::Expected<GPUBuffer, lib::Result<GPUBufferError>> cbuffer(backend::Device& device, uint32_t const size, backend::BufferFlags const flags);

    static lib::Expected<GPUBuffer, lib::Result<GPUBufferError>> sbuffer(backend::Device& device, uint32_t const size, backend::BufferFlags const flags, uint32_t const element_stride);
};

template<>
struct GPUResourceDeleter<GPUBuffer> {
    void operator()(backend::Device& device, GPUBuffer const& buffer) const {
        device.delete_buffer(buffer.buffer);
    }
};

}
