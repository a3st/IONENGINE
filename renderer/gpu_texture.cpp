// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_texture.h>

using namespace ionengine;
using namespace ionengine::renderer;

std::shared_ptr<GPUTexture> GPUTexture::render_target(
    backend::Device& device, 
    backend::Format const format, 
    uint32_t const width, 
    uint32_t const height
) {

    return std::shared_ptr<GPUTexture>(new GPUTexture(device, format, width, height, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource));
}

GPUTexture::GPUTexture(
    backend::Device& device, 
    backend::Format const format, 
    uint32_t const width, 
    uint32_t const height, 
    backend::TextureFlags const flags
) : _device(&device) {

    _texture = _device->create_texture(
        backend::Dimension::_2D, 
        width, 
        height, 
        1, 
        1, 
        format,
        flags
    );

    _memory_state = backend::MemoryState::RenderTarget;

    _sampler = _device->create_sampler(
        backend::Filter::MinMagMipLinear,
        backend::AddressMode::Clamp,
        backend::AddressMode::Clamp,
        backend::AddressMode::Clamp,
        1,
        backend::CompareOp::Always
    );
}

GPUTexture::~GPUTexture() {
    _device->delete_texture(_texture);
    _device->delete_sampler(_sampler);
}

backend::Handle<backend::Texture> GPUTexture::as_texture() const {
    assert(_texture != backend::InvalidHandle<backend::Texture>() && "GPUTexture is invalid as texture");
    return _texture; 
}

backend::Handle<backend::Sampler> GPUTexture::as_sampler() const { 
    assert(_sampler != backend::InvalidHandle<backend::Sampler>() && "GPUTexture is invalid as sampler");
    return _sampler; 
}

bool GPUTexture::is_render_target() const {
    return _flags & backend::TextureFlags::RenderTarget;
}

bool GPUTexture::is_depth_stencil() const {
    return _flags & backend::TextureFlags::DepthStencil;
}

void GPUTexture::barrier(backend::Handle<backend::CommandList> const& command_list, backend::MemoryState const memory_state) {
    _device->barrier(command_list, _texture, _memory_state, memory_state);
    _memory_state = memory_state;
}

backend::MemoryState GPUTexture::memory_state() const {
    return _memory_state;
}

