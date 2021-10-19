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

    Texture();
    Texture(const Texture&) = delete;
    Texture(Texture&& rhs) noexcept;
    Texture(Device* device, const uint32_t buffer_index);

    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&& rhs) noexcept;

    inline uint32_t GetMipLevels() const { return mip_levels_; }
    inline uint32_t GetArrayLayers() const { return array_layers_; }
    inline uint32_t GetWidth() const { return width_; }
    inline uint32_t GetHeight() const { return height_; }
    inline TextureFlags GetFlags() const { return flags_; }
    inline Format GetFormat() const { return format_; }

private:

    uint32_t mip_levels_;
    uint32_t array_layers_;
    uint32_t width_;
    uint32_t height_;
    TextureFlags flags_;
    Format format_;

    ComPtr<ID3D12Resource> resource_;
    D3D12_RESOURCE_DESC resource_desc_;

    MemoryPtr ptr_;
    MemoryPool* pool_;
};

}