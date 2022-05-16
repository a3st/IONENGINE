// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/upload_context.h>

namespace ionengine::renderer {

class GPUBuffer {
public:

    ~GPUBuffer();

    static std::shared_ptr<GPUBuffer> cbuffer(backend::Device& device, uint32_t const size);

    backend::Handle<backend::Buffer> as_buffer() const;

    void copy_data(UploadContext& context, std::span<uint8_t const> const data);

    bool is_cbuffer() const;

private:

    GPUBuffer(backend::Device& device, size_t const size, backend::BufferFlags const flags);

    backend::Device* _device;

    backend::Handle<backend::Buffer> _buffer;

    backend::BufferFlags _flags;

    backend::MemoryState _memory_state;
};

}