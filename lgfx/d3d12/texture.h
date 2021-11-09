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

    Texture() = default;

    Texture(Device* const device, const uint32_t buffer_index);

    Texture(
        Device* const device, MemoryPool* const pool, 
        const Dimension dimension,
        const uint32_t width, const uint32_t height,
        const uint32_t mip_levels, const uint32_t array_layers,
        const Format format,
        const TextureFlags flags);

    Texture(const Texture&) = delete;
    
    Texture(Texture&& rhs) noexcept = default;

    Texture& operator=(const Texture&) = delete;

    Texture& operator=(Texture&& rhs) noexcept = default;

    inline Dimension GetDimension() const { return dimension_; }
    inline uint32_t GetWidth() const { return width_; }
    inline uint32_t GetHeight() const { return height_; }
    inline uint32_t GetMipLevels() const { return mip_levels_; }
    inline uint32_t GetArrayLayers() const { return array_layers_; }
    inline Format GetFormat() const { return format_; }
    inline TextureFlags GetFlags() const { return flags_; }

private:

    MemoryPool* pool_;

    ComPtr<ID3D12Resource> resource_;

    MemoryAllocInfo alloc_info_;

    Dimension dimension_;
    uint32_t width_;
    uint32_t height_;
    uint32_t mip_levels_;
    uint32_t array_layers_;
    Format format_;
    TextureFlags flags_;
};

}