// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/upload_context.h>
#include <asset/texture.h>

namespace ionengine::renderer {

class GPUTexture {
public:

    ~GPUTexture();

    static std::shared_ptr<GPUTexture> render_target(backend::Device& device, backend::Format const format, uint32_t const width, uint32_t const height, backend::TextureFlags const flags);

    static std::shared_ptr<GPUTexture> depth_stencil(backend::Device& device, backend::Format const format, uint32_t const width, uint32_t const height);

    static std::shared_ptr<GPUTexture> sampler(backend::Device& device, UploadContext& upload_context, asset::Texture const& texture);

    void copy_texture_data(UploadContext& context, std::pair<uint32_t, uint32_t> const mip_range, std::span<uint8_t const> const data);

    backend::Handle<backend::Texture> as_texture() const;

    backend::Handle<backend::Sampler> as_sampler() const;

    bool is_render_target() const;

    bool is_depth_stencil() const;

    bool is_unordered_access() const;

    void barrier(backend::Handle<backend::CommandList> const& command_list, backend::MemoryState const memory_state);

    backend::MemoryState memory_state() const;

private:

    GPUTexture(
        backend::Device& device, 
        backend::Format const format, 
        uint32_t const width, 
        uint32_t const height, 
        uint32_t const mip_count,
        backend::TextureFlags const flags
    );

    backend::Device* _device;

    backend::Handle<backend::Texture> _texture;
    backend::Handle<backend::Sampler> _sampler;

    uint32_t _width;
    uint32_t _height;

    backend::TextureFlags _flags;
    backend::Format _format;

    backend::MemoryState _memory_state;

    static backend::Format constexpr get_texture_format(asset::TextureFormat const format);
};

}
