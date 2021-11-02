// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_view.h"
#include "texture.h"
#include "device.h"

using namespace lgfx;

DescriptorAllocInfo TextureView::CreateRenderTarget(Device* device) {

    DescriptorAllocInfo alloc_info = pool_->Allocate();

    D3D12_RENDER_TARGET_VIEW_DESC view_desc{};
    view_desc.Format = texture_->resource_desc_.Format;

    switch(desc_.dimension) {
        case Dimension::kTexture2D: {
            view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            view_desc.Texture2D.MipSlice = desc_.base_mip_level;
            view_desc.Texture2D.PlaneSlice = desc_.base_array_layer;
            break;
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device->rtv_descriptor_offset_ };
    device->device_->CreateRenderTargetView(texture_->resource_.Get(), &view_desc, cpu_handle);
    return alloc_info;
}

DescriptorAllocInfo TextureView::CreateShaderResource(Device* device) {

    DescriptorAllocInfo alloc_info = pool_->Allocate();

    D3D12_SHADER_RESOURCE_VIEW_DESC view_desc{};
    view_desc.Format = texture_->resource_desc_.Format;

    switch(desc_.dimension) {

        case Dimension::kTexture2D: {
            view_desc.Texture2D.PlaneSlice = desc_.base_array_layer;
            view_desc.Texture2D.MipLevels = desc_.mip_level_count;
            view_desc.Texture2D.MostDetailedMip = desc_.base_mip_level;
            break;
        }
    }
    
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device->rtv_descriptor_offset_ };
    device->device_->CreateShaderResourceView(texture_->resource_.Get(), &view_desc, cpu_handle);
    return alloc_info;
}

TextureView::TextureView(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc) : 
    pool_(pool), texture_(texture), desc_(desc) {

    TextureFlags flags = texture->GetDesc().flags;
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