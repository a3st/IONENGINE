// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "texture.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Texture::Texture(
    rhi::Device& device,
    core::ref_ptr<rhi::MemoryAllocator> allocator,
    uint32_t const width,
    uint32_t const height,
    uint32_t const depth,
    uint32_t const mip_levels,
    rhi::TextureFormat const format,
    rhi::TextureDimension const dimension,
    std::vector<std::span<uint8_t const>> const& data,
    bool const immediate
) :
    texture(
        device.create_texture(
            allocator,
            width,
            height,
            depth,
            mip_levels,
            format,
            dimension,
            (rhi::TextureUsageFlags)(rhi::TextureUsage::CopyDst | rhi::TextureUsage::ShaderResource),
            data
        )
    )
{
    if(immediate) {
        texture.wait();
    }
}