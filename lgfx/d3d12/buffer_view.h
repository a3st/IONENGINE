// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

class BufferView {

friend class CommandBuffer;
friend class DescriptorSet;

public:

    BufferView(
        Device* device, DescriptorPool* pool, 
        Buffer* buffer, 
        const Format index_format,
        const uint32_t stride);

    ~BufferView();
    
    BufferView(const TextureView&) = delete;
    BufferView(TextureView&&) = delete;

    BufferView& operator=(const BufferView&) = delete;
    BufferView& operator=(BufferView&&) = delete;

    inline Buffer* GetBuffer() const { return buffer_; }

private:

    DescriptorAllocInfo CreateConstantBuffer(Device* device);
    D3D12_INDEX_BUFFER_VIEW CreateIndexBuffer();
    D3D12_VERTEX_BUFFER_VIEW CreateVertexBuffer();

    DescriptorPool* pool_;
    Buffer* buffer_;

    union {
        D3D12_INDEX_BUFFER_VIEW index_view_desc_;
        D3D12_VERTEX_BUFFER_VIEW vertex_view_desc_;
    };

    DescriptorAllocInfo alloc_info_;
    
    Format index_format_;
    uint32_t stride_;
};

}