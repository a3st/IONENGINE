// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_texture.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GPUTexture, lib::Result<GPUTextureError>> GPUTexture::create(
    backend::Device& device,
    backend::Format const format, 
    uint32_t const width, 
    uint32_t const height, 
    uint32_t const depth,
    uint32_t const mip_count,
    backend::TextureFlags const flags
) {

    auto gpu_texture = GPUTexture {};
    {
        gpu_texture.texture = device.create_texture(
            backend::Dimension::_2D, 
            width, 
            height, 
            mip_count,
            depth, 
            format,
            flags
        );
        gpu_texture.format = format;
        gpu_texture.width = width;
        gpu_texture.height = height;
        gpu_texture.mip_count = mip_count;
        gpu_texture.flags = flags;

        if(flags & backend::TextureFlags::RenderTarget) {
            gpu_texture.memory_state = backend::MemoryState::RenderTarget;
        } else if(flags & backend::TextureFlags::DepthStencil) {
            gpu_texture.memory_state = backend::MemoryState::DepthWrite;
        } else if(flags & backend::TextureFlags::HostWrite) {
            gpu_texture.memory_state = backend::MemoryState::GenericRead;
        } else {
            gpu_texture.memory_state = backend::MemoryState::Common;
        }

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
    }
    return lib::Expected<GPUTexture, lib::Result<GPUTextureError>>::ok(std::move(gpu_texture));
}

lib::Expected<GPUTexture, lib::Result<GPUTextureError>> GPUTexture::load_from_texture(backend::Device& device, asset::Texture const& texture) {

    auto gpu_texture = GPUTexture {};
    {
        gpu_texture.texture = device.create_texture(
            backend::Dimension::_2D, 
            texture.width, 
            texture.height, 
            texture.mip_count,
            1, 
            get_texture_format(texture.format),
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

        gpu_texture.format = get_texture_format(texture.format);
        gpu_texture.width = texture.width;
        gpu_texture.height = texture.height;
        gpu_texture.mip_count = texture.mip_count;
        gpu_texture.memory_state = backend::MemoryState::Common;
        gpu_texture.flags = backend::TextureFlags::ShaderResource;
    }
    return lib::Expected<GPUTexture, lib::Result<GPUTextureError>>::ok(std::move(gpu_texture));
}

backend::Format constexpr ionengine::renderer::get_texture_format(asset::TextureFormat const format) {
    switch(format) {
        case asset::TextureFormat::BC1: return backend::Format::BC1;
        case asset::TextureFormat::BC3: return backend::Format::BC3;
        case asset::TextureFormat::BC5: return backend::Format::BC5;
        case asset::TextureFormat::BC4: return backend::Format::BC4;
        case asset::TextureFormat::RGBA8_UNORM: return backend::Format::RGBA8_UNORM;
        default: return backend::Format::Unknown;
    }
}

backend::AddressMode constexpr ionengine::renderer::get_texture_address(asset::TextureAddress const address) {
    switch(address) {
        case asset::TextureAddress::Clamp: return backend::AddressMode::Clamp;
        case asset::TextureAddress::Mirror: return backend::AddressMode::Mirror;
        case asset::TextureAddress::Wrap: return backend::AddressMode::Wrap;
        default: {
            assert(false && "invalid data type");
            return backend::AddressMode::Clamp;
        }
    }
}