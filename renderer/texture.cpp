// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "texture.hpp"
#include "backend.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

Texture::Texture(
    Backend& backend,
    uint32_t const width,
    uint32_t const height,
    uint32_t const depth,
    uint32_t const mip_levels,
    wgpu::TextureFormat const format,
    TextureDimension const dimension,
    uint32_t const sample_count,
    wgpu::TextureUsageFlags const usage
) : 
    backend(&backend) 
{
    {
        auto descriptor = wgpu::TextureDescriptor {};
        descriptor.size = wgpu::Extent3D(width, height, depth);
        descriptor.mipLevelCount = mip_levels;
        descriptor.format = format;
        switch(dimension) {
            case TextureDimension::_2D: {
                descriptor.dimension = wgpu::TextureDimension::_2D;
            } break;
            case TextureDimension::Cube: {
                descriptor.dimension = wgpu::TextureDimension::_2D;
            } break;
            case TextureDimension::_3D: {
                descriptor.dimension = wgpu::TextureDimension::_3D;
            } break;
        }
        descriptor.sampleCount = sample_count;
        descriptor.usage = usage;

        texture = backend.get_device().createTexture(descriptor);
    }

    if(usage & wgpu::TextureUsage::TextureBinding)
    {
        auto descriptor = wgpu::SamplerDescriptor {};
    }

    {
        auto descriptor = wgpu::TextureViewDescriptor {};
        descriptor.aspect = wgpu::TextureAspect::All;
        descriptor.baseArrayLayer = 0;
        descriptor.arrayLayerCount = depth;
        descriptor.baseMipLevel = 0;
        descriptor.mipLevelCount = mip_levels;
        descriptor.format = format;
        switch(dimension) {
            case TextureDimension::_2D: {
                descriptor.dimension = wgpu::TextureViewDimension::_2D;
            } break;
            case TextureDimension::Cube: {
                descriptor.dimension = wgpu::TextureViewDimension::Cube;
            } break;
            case TextureDimension::_3D: {
                descriptor.dimension = wgpu::TextureViewDimension::_3D;
            } break;
        }
        
        view = texture.createView(descriptor);
    }
}

