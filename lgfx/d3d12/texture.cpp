// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture.h"
#include "conversion.h"
#include "device.h"

using namespace lgfx;

Texture::Texture() {

}

Texture::~Texture() {

    resource_->Release();
}

Texture::Texture(Texture&& rhs) noexcept {

    resource_ = rhs.resource_;
    resource_desc_ = std::move(rhs.resource_desc_);

    width_ = rhs.width_;
    height_ = rhs.height_;
    format_ = rhs.format_;
    flags_ = rhs.flags_;
    array_layers_ = rhs.array_layers_;
    mip_levels_ = rhs.mip_levels_;

    ptr_ = rhs.ptr_;
    pool_ = rhs.pool_;
}

Texture& Texture::operator=(Texture&& rhs) noexcept {

    resource_ = rhs.resource_;
    resource_desc_ = std::move(rhs.resource_desc_);

    width_ = rhs.width_;
    height_ = rhs.height_;
    format_ = rhs.format_;
    flags_ = rhs.flags_;
    array_layers_ = rhs.array_layers_;
    mip_levels_ = rhs.mip_levels_;

    ptr_ = rhs.ptr_;
    pool_ = rhs.pool_;
    return *this;
}

Texture::Texture(Device* device, const uint32_t buffer_index) {

    THROW_IF_FAILED(device->swapchain_->GetBuffer(buffer_index, __uuidof(ID3D12Resource), reinterpret_cast<void**>(&resource_)));
    resource_desc_ = resource_->GetDesc();

    width_ = static_cast<uint32_t>(resource_desc_.Width);
    height_ = resource_desc_.Height;
    format_ = DXGIFormatTo(resource_desc_.Format);
    flags_ = TextureFlags::kRenderTarget;
    array_layers_ = resource_desc_.DepthOrArraySize;
    mip_levels_ = resource_desc_.MipLevels;
}

uint32_t Texture::GetArrayLayers() const {

    return array_layers_;
}

uint32_t Texture::GetMipLevels() const { 
    
    return mip_levels_;
}

uint32_t Texture::GetWidth() const {

    return width_;
}

uint32_t Texture::GetHeight() const {

    return height_;
}

TextureFlags Texture::GetFlags() const {

    return flags_;
}

Format Texture::GetFormat() const {

    return format_;
}