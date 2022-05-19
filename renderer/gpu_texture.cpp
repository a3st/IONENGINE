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

    return std::shared_ptr<GPUTexture>(new GPUTexture(device, format, width, height, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource));
}

std::shared_ptr<GPUTexture> GPUTexture::depth_stencil(
    backend::Device& device, 
    backend::Format const format, 
    uint32_t const width, 
    uint32_t const height
) {

    return std::shared_ptr<GPUTexture>(new GPUTexture(device, format, width, height, 1, backend::TextureFlags::DepthStencil));
}

std::shared_ptr<GPUTexture> GPUTexture::sampler(backend::Device& device, UploadContext& upload_context, asset::Texture const& texture) {

    auto gpu_texture = std::shared_ptr<GPUTexture>(
        new GPUTexture(
            device, 
            get_texture_format(texture.format()), 
            texture.width(), 
            texture.height(), 
            texture.mip_count(), 
            backend::TextureFlags::ShaderResource
        )
    );

    gpu_texture->copy_texture_data(upload_context, { 0, 1 }, texture.data());
    return gpu_texture;
}

GPUTexture::GPUTexture(
    backend::Device& device, 
    backend::Format const format, 
    uint32_t const width, 
    uint32_t const height,
    uint32_t const mip_count,
    backend::TextureFlags const flags
) : _device(&device) {

    _texture = _device->create_texture(
        backend::Dimension::_2D, 
        width, 
        height, 
        1 /*mip_count*/, 
        1, 
        format,
        flags
    );

    _flags = flags;

    if(flags & backend::TextureFlags::RenderTarget) {
        _memory_state = backend::MemoryState::RenderTarget;
    } else {
        _memory_state = backend::MemoryState::Common;
    }

    _sampler = _device->create_sampler(
        backend::Filter::MinMagMipLinear,
        backend::AddressMode::Clamp,
        backend::AddressMode::Clamp,
        backend::AddressMode::Clamp,
        2,
        backend::CompareOp::Always
    );
}

GPUTexture::~GPUTexture() {
    _device->delete_texture(_texture);
    _device->delete_sampler(_sampler);
}

void GPUTexture::copy_texture_data(UploadContext& context, std::pair<uint32_t, uint32_t> const mip_range, std::span<uint8_t const> const data) {
    context.begin();
    context.copy_texture_data(_texture, mip_range, data);
    context.end();
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

backend::Format constexpr GPUTexture::get_texture_format(asset::TextureFormat const format) {
    switch(format) {
        case asset::TextureFormat::BC1: return backend::Format::BC1;
        case asset::TextureFormat::BC5: return backend::Format::BC5;
        default: return backend::Format::Unknown;
    }
}