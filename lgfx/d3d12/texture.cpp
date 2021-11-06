// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture.h"
#include "conversion.h"
#include "device.h"

using namespace lgfx;

Texture::Texture(Device* const device, const uint32_t buffer_index) : pool_(nullptr) {

    THROW_IF_FAILED(device->swapchain_->GetBuffer(buffer_index, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource_.GetAddressOf())));
    D3D12_RESOURCE_DESC resource_desc = resource_->GetDesc();

    width_ = static_cast<uint32_t>(resource_desc.Width);
    height_ = resource_desc.Height;
    array_layers_ = resource_desc.DepthOrArraySize;
    mip_levels_ = resource_desc.MipLevels;
    format_ = DXGIFormatTo(resource_desc.Format);
    flags_ = TextureFlags::kRenderTarget;
}

Texture::Texture(
    Device* const device, MemoryPool* const pool, 
    const Dimension dimension,
    const uint32_t width, const uint32_t height,
    const uint32_t mip_levels, const uint32_t array_layers,
    const Format format,
    const TextureFlags flags) : 
        pool_(pool),
        dimension_(dimension),
        width_(width), height_(height),
        mip_levels_(mip_levels), array_layers_(array_layers),
        format_(format), flags_(flags) {

    D3D12_RESOURCE_STATES initial_state;
    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = ToD3D12ResourceDimension(dimension_);
    resource_desc.Width = width_;
    resource_desc.Height = height_;
    resource_desc.MipLevels = mip_levels_;
    resource_desc.DepthOrArraySize = array_layers_;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = ToDXGIFormat(format_);
    if(flags_ & TextureFlags::kRenderTarget) {
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else if(flags_ & TextureFlags::kDepthStencil) {
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else if(flags_ & TextureFlags::kUnorderedAccess) {
        resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    } else if(flags & TextureFlags::kShaderResource) {
        
    }

    D3D12_RESOURCE_ALLOCATION_INFO alloc_info = device->device_->GetResourceAllocationInfo(0, 1, &resource_desc);
    alloc_info_ = pool_->Allocate(alloc_info.SizeInBytes + alloc_info.Alignment);
    
    THROW_IF_FAILED(device->device_->CreatePlacedResource(alloc_info_.heap->heap_.Get(), alloc_info_.offset, &resource_desc, initial_state, nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource_.GetAddressOf())));
}

Texture::~Texture() {

    if(pool_) {
        pool_->Deallocate(alloc_info_);
    }
}

Texture::Texture(Texture&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    resource_.Swap(rhs.resource_);
    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(dimension_, rhs.dimension_);
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(mip_levels_, rhs.mip_levels_);
    std::swap(array_layers_, rhs.array_layers_);
    std::swap(format_, rhs.format_);
    std::swap(flags_, rhs.flags_);
}

Texture& Texture::operator=(Texture&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    resource_.Swap(rhs.resource_);
    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(dimension_, rhs.dimension_);
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(mip_levels_, rhs.mip_levels_);
    std::swap(array_layers_, rhs.array_layers_);
    std::swap(format_, rhs.format_);
    std::swap(flags_, rhs.flags_);
    return *this;
}