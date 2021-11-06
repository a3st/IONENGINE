// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_view.h"
#include "texture.h"
#include "device.h"
#include "conversion.h"

using namespace lgfx;

DescriptorAllocInfo TextureView::CreateRenderTarget(Device* const device) {

    DescriptorAllocInfo alloc_info = pool_->Allocate();

    D3D12_RENDER_TARGET_VIEW_DESC view_desc{};
    view_desc.Format = ToDXGIFormat(texture_->GetFormat());

    switch(dimension_) {
        case Dimension::kTexture2D: {
            view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            view_desc.Texture2D.MipSlice = base_mip_level_;
            view_desc.Texture2D.PlaneSlice = base_array_layer_;
            break;
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device->rtv_descriptor_offset_ };
    device->device_->CreateRenderTargetView(texture_->resource_.Get(), &view_desc, cpu_handle);
    return alloc_info;
}

DescriptorAllocInfo TextureView::CreateShaderResource(Device* const device) {

    DescriptorAllocInfo alloc_info = pool_->Allocate();

    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc{};
    view_desc.Format = ToDXGIFormat(texture_->GetFormat());

    switch(dimension_) {

        case Dimension::kTexture2D: {
            view_desc.Texture2D.PlaneSlice = base_array_layer_;
            view_desc.Texture2D.MipLevels = mip_level_count_;
            view_desc.Texture2D.MostDetailedMip = base_mip_level_;
            break;
        }
    }
    
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device->rtv_descriptor_offset_ };
    device->device_->CreateShaderResourceView(texture_->resource_.Get(), &view_desc, cpu_handle);
    return alloc_info;
}

TextureView::TextureView(
    Device* const device, DescriptorPool* const pool, 
    Texture* const texture, 
    const Dimension dimension,
    const uint32_t base_mip_level, const uint32_t mip_level_count,
    const uint32_t base_array_layer, const uint32_t array_layer_count) :
        pool_(pool), 
        texture_(texture),
        dimension_(dimension),
        base_mip_level_(base_mip_level), mip_level_count_(mip_level_count),
        base_array_layer_(base_array_layer), array_layer_count_(array_layer_count) {

    TextureFlags flags = texture->GetFlags();
    if(flags & TextureFlags::kRenderTarget) {
        alloc_info_ = CreateRenderTarget(device);
    } else if(flags & TextureFlags::kShaderResource) {
        alloc_info_ = CreateShaderResource(device);
    }
}

TextureView::~TextureView() {

    if(pool_) {
        pool_->Deallocate(alloc_info_);
    }
}

TextureView::TextureView(TextureView&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    std::swap(texture_, rhs.texture_);
    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(dimension_, rhs.dimension_);
    std::swap(base_mip_level_, rhs.base_mip_level_);
    std::swap(mip_level_count_, rhs.mip_level_count_);
    std::swap(base_array_layer_, rhs.base_array_layer_);
    std::swap(array_layer_count_, rhs.array_layer_count_);
}

TextureView& TextureView::operator=(TextureView&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    std::swap(texture_, rhs.texture_);
    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(dimension_, rhs.dimension_);
    std::swap(base_mip_level_, rhs.base_mip_level_);
    std::swap(mip_level_count_, rhs.mip_level_count_);
    std::swap(base_array_layer_, rhs.base_array_layer_);
    std::swap(array_layer_count_, rhs.array_layer_count_);
    return *this;
}