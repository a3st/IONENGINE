// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_view.h"
#include "texture.h"
#include "device.h"

using namespace lgfx;

DescriptorPtr TextureView::CreateRenderTarget(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc) {

    DescriptorPtr ptr = pool->Allocate();

    D3D12_RENDER_TARGET_VIEW_DESC view_desc{};
    view_desc.Format = texture->resource_desc_.Format;

    switch(desc.dimension) {
        case Dimension::kTexture2D: {
            view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
            view_desc.Texture2D.MipSlice = desc.base_mip_level;
            view_desc.Texture2D.PlaneSlice = desc.base_array_layer;
            break;
        }
    }

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { ptr.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + ptr.offset * device->rtv_descriptor_offset_ };
    device->device_->CreateRenderTargetView(texture->resource_.Get(), &view_desc, cpu_handle);
    return ptr;
}

TextureView::TextureView(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc) : pool_(pool), texture_(texture), view_desc_(desc) {

    TextureFlags flags = texture->GetDesc().flags;
    if(flags & TextureFlags::kRenderTarget) {
        ptr_ = CreateRenderTarget(device, pool, texture, desc);
    }
}