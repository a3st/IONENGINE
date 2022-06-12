// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <asset/texture.h>

namespace ionengine::renderer {

enum class GPUTextureError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

struct GPUTexture {
    backend::Handle<backend::Texture> texture;
    backend::Handle<backend::Sampler> sampler;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_count;
    backend::TextureFlags flags;
    backend::Format format;
    backend::AddressMode s_address_mode;
    backend::AddressMode t_address_mode;
    backend::MemoryState memory_state;
    bool is_swapchain;

    bool is_render_target() const {
        return flags & backend::TextureFlags::RenderTarget;
    }

    bool is_sampler() const {
        return flags & backend::TextureFlags::ShaderResource;
    }

    bool is_depth_stencil() const {
        return flags & backend::TextureFlags::DepthStencil;
    }

    bool is_rw_texture() const {
        return flags & backend::TextureFlags::UnorderedAccess;
    }

    backend::MemoryBarrierDesc barrier(backend::MemoryState const after);

    void recreate_sampler(backend::Filter const filter, uint16_t const anisotropic);

    static lib::Expected<GPUTexture, lib::Result<GPUTextureError>> create(
        backend::Device& device,
        backend::Format const format, 
        uint32_t const width, 
        uint32_t const height, 
        uint32_t const depth,
        uint32_t const mip_count,
        backend::TextureFlags const flags
    );

    static lib::Expected<GPUTexture, lib::Result<GPUTextureError>> load_from_texture(backend::Device& device, asset::Texture const& texture);
};

template<>
struct GPUResourceDeleter<GPUTexture> {
    void operator()(backend::Device& device, GPUTexture const& texture) const {
        device.delete_texture(texture.texture);
        if(texture.is_sampler()) {
            device.delete_sampler(texture.sampler);
        }
    }
};

backend::Format constexpr get_texture_format(asset::TextureFormat const format);

backend::AddressMode constexpr get_texture_address(asset::TextureAddress const address);

}
