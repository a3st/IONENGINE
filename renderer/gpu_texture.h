// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>

namespace ionengine::renderer {

class GPUTexture {
public:

    ~GPUTexture();

    static std::shared_ptr<GPUTexture> render_target(backend::Device& device, backend::Format const format, uint32_t const width, uint32_t const height);

    backend::Handle<backend::Texture> as_texture() const;

    backend::Handle<backend::Sampler> as_sampler() const;

    bool is_render_target() const;

    bool is_depth_stencil() const;

    void barrier(backend::Handle<backend::CommandList> const& command_list, backend::MemoryState const memory_state);

    backend::MemoryState memory_state() const;

private:

    GPUTexture(backend::Device& device, backend::Format const format, uint32_t const width, uint32_t const height, backend::TextureFlags const flags);

    backend::Device* _device;

    backend::Handle<backend::Texture> _texture;
    backend::Handle<backend::Sampler> _sampler;

    backend::TextureFlags _flags;

    backend::MemoryState _memory_state;
};

}
