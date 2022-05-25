// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_texture.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GPUTexture, lib::Result<GPUTextureError>> GPUTexture::render_target(
    backend::Device& device,
    backend::Format const format, 
    uint32_t const width, 
    uint32_t const height, 
    backend::TextureFlags const flags
) {

    auto gpu_texture = GPUTexture {};
    {
        gpu_texture.texture = device.create_texture(
            backend::Dimension::_2D, 
            width, 
            height, 
            1,
            1, 
            format,
            backend::TextureFlags::RenderTarget | flags
        );

        if(flags & backend::TextureFlags::ShaderResource) {
            gpu_texture.sampler = device.create_sampler(
                backend::Filter::MinMagMipLinear,
                backend::AddressMode::Clamp,
                backend::AddressMode::Clamp,
                backend::AddressMode::Clamp,
                2,
                backend::CompareOp::Always
            );
        }

        gpu_texture.format = format;
        gpu_texture.width = width;
        gpu_texture.height = height;
        gpu_texture.mip_count = 1;

        gpu_texture.memory_state = backend::MemoryState::Common;
        gpu_texture.flags = backend::TextureFlags::RenderTarget | flags;
    }
    return lib::Expected<GPUTexture, lib::Result<GPUTextureError>>::ok(std::move(gpu_texture));
}

lib::Expected<GPUTexture, lib::Result<GPUTextureError>> GPUTexture::depth_stencil(
    backend::Device& device,
    backend::Format const format, 
    uint32_t const width, 
    uint32_t const height, 
    backend::TextureFlags const flags
) {

    auto gpu_texture = GPUTexture {};
    {
        gpu_texture.texture = device.create_texture(
            backend::Dimension::_2D, 
            width, 
            height, 
            1,
            1, 
            format,
            backend::TextureFlags::DepthStencil | flags
        );

        if(flags & backend::TextureFlags::ShaderResource) {
            gpu_texture.sampler = device.create_sampler(
                backend::Filter::MinMagMipLinear,
                backend::AddressMode::Clamp,
                backend::AddressMode::Clamp,
                backend::AddressMode::Clamp,
                2,
                backend::CompareOp::Always
            );
        }

        gpu_texture.format = format;
        gpu_texture.width = width;
        gpu_texture.height = height;
        gpu_texture.mip_count = 1;

        gpu_texture.memory_state = backend::MemoryState::Common;
        gpu_texture.flags = backend::TextureFlags::DepthStencil | flags;
    }
    return lib::Expected<GPUTexture, lib::Result<GPUTextureError>>::ok(std::move(gpu_texture));
}

lib::Expected<GPUTexture, lib::Result<GPUTextureError>> GPUTexture::load_from_texture(backend::Device& device, asset::Texture const& texture) {

    auto gpu_texture = GPUTexture {};
    {
        gpu_texture.texture = device.create_texture(
            backend::Dimension::_2D, 
            texture.width(), 
            texture.height(), 
            texture.mip_count(),
            1, 
            get_texture_format(texture.format()),
            backend::TextureFlags::ShaderResource
        );

        gpu_texture.sampler = device.create_sampler(
            backend::Filter::MinMagMipLinear,
            backend::AddressMode::Wrap,
            backend::AddressMode::Wrap,
            backend::AddressMode::Wrap,
            2,
            backend::CompareOp::Always
        );

        gpu_texture.format = get_texture_format(texture.format());
        gpu_texture.width = texture.width();
        gpu_texture.height = texture.height();
        gpu_texture.mip_count = 1;

        gpu_texture.memory_state = backend::MemoryState::Common;
        gpu_texture.flags = backend::TextureFlags::ShaderResource;
    }
    return lib::Expected<GPUTexture, lib::Result<GPUTextureError>>::ok(std::move(gpu_texture));
}

backend::Format constexpr ionengine::renderer::get_texture_format(asset::TextureFormat const format) {
    switch(format) {
        case asset::TextureFormat::BC1: return backend::Format::BC1;
        case asset::TextureFormat::BC5: return backend::Format::BC5;
        case asset::TextureFormat::BC4: return backend::Format::BC4;
        case asset::TextureFormat::RGBA8_UNORM: return backend::Format::RGBA8;
        default: return backend::Format::Unknown;
    }
}
