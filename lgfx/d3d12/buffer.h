// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "memory_pool.h"

namespace lgfx {

class Buffer {

friend class BufferView;
friend class CommandBuffer;

public:

    Buffer(Device* device, MemoryPool* pool, const BufferDesc& desc);
    ~Buffer();
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&) = delete;

    inline const BufferDesc& GetDesc() const { return desc_; }

    std::byte* Map();
    void Unmap();

private:

    ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_DESC resource_desc_;
    D3D12_RESOURCE_STATES initial_state_;

    MemoryAllocInfo alloc_info_;
    MemoryPool* pool_;

    BufferDesc desc_;
};

}