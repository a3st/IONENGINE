// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

class TextureView {

friend class CommandBuffer;
friend class DescriptorSet;

public:

    TextureView(
        Device* device, DescriptorPool* pool, 
        Texture* texture, 
        const Dimension dimension,
        const uint32_t base_mip_level, const uint32_t mip_level_count,
        const uint32_t base_array_layer, const uint32_t array_layer_count);

    ~TextureView();

    TextureView(const TextureView&) = delete;
    TextureView(TextureView&&) = delete;

    TextureView& operator=(const TextureView&) = delete;
    TextureView& operator=(TextureView&&) = delete;

    inline Texture* GetTexture() const { return texture_; }
    inline Dimension GetDimension() const { return dimension_; }
    inline uint32_t GetBaseMipLevel() const { return base_mip_level_; }
    inline uint32_t GetMipLevelCount() const { return mip_level_count_; }
    inline uint32_t GetBaseArrayLayer() const { return base_array_layer_; }
    inline uint32_t GetArrayLayerCount() const { return array_layer_count_; }

private:

    DescriptorAllocInfo CreateRenderTarget(Device* device);
    DescriptorAllocInfo CreateShaderResource(Device* device);

    DescriptorPool* pool_;
    Texture* texture_;

    DescriptorAllocInfo alloc_info_;

    Dimension dimension_;
    uint32_t base_mip_level_;
    uint32_t mip_level_count_;
    uint32_t base_array_layer_;
    uint32_t array_layer_count_;
};

}