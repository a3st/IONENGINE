// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/upload_context.h>

namespace ionengine::renderer {

class GPUBuffer {
public:

    ~GPUBuffer();

    static std::shared_ptr<GPUBuffer> cbuffer(backend::Device& device, uint32_t const size);

    static std::shared_ptr<GPUBuffer> sbuffer(backend::Device& device, uint32_t const size, uint32_t const element_stride);

    backend::Handle<backend::Buffer> as_buffer() const;

    void copy_data(UploadContext& context, std::span<uint8_t const> const data);

    bool is_cbuffer() const;

    void barrier(backend::Handle<backend::CommandList> const& command_list, backend::MemoryState const memory_state);

    backend::MemoryState memory_state() const;

private:

    GPUBuffer(backend::Device& device, size_t const size, backend::BufferFlags const flags, uint32_t const element_stride);

    backend::Device* _device;

    backend::Handle<backend::Buffer> _buffer;

    backend::BufferFlags _flags;

    backend::MemoryState _memory_state;
};

}