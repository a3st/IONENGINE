// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "render_target.hpp"
#include "precompiled.h"

namespace ionengine::renderer
{
    RenderTarget::RenderTarget(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator, uint32_t const width,
                               uint32_t const height)
        : width(width), height(height)
    {
        for (uint32_t i = 0; i < 2; ++i)
        {
            auto texture =
                device
                    .create_texture(
                        allocator, width, height, 1, 1, rhi::TextureFormat::RGBA8_UNORM, rhi::TextureDimension::_2D,
                        (rhi::TextureUsageFlags)(rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource),
                        {})
                    .get();
            buffers.emplace_back(texture);
        }
    }
} // namespace ionengine::renderer