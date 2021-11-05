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

    Buffer(
        Device* const device, 
        MemoryPool* const pool, 
        const size_t size, 
        const BufferFlags flags);

    ~Buffer();
    
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&& rhs) noexcept;

    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&& rhs) noexcept;

    std::byte* Map();
    void Unmap();

    inline size_t GetSize() const { return size_; }
    inline BufferFlags GetFlags() const { return flags_; }

private:

    MemoryPool* pool_;

    ComPtr<ID3D12Resource> resource_;

    MemoryAllocInfo alloc_info_;

    size_t size_;
    BufferFlags flags_;
};

}