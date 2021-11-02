// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "memory_pool.h"

namespace lgfx {

class Texture {

friend class TextureView;
friend class CommandBuffer;

public:

    Texture(Device* device, const uint32_t buffer_index);
    Texture(Device* device, MemoryPool* pool, const TextureDesc& desc);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture(Texture&&) = delete;

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&) = delete;

    inline const TextureDesc& GetDesc() const { return desc_; }

private:

    ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_DESC resource_desc_;
    D3D12_RESOURCE_STATES initial_state_;

    MemoryAllocInfo alloc_info_;
    MemoryPool* pool_;

    TextureDesc desc_;
};

}