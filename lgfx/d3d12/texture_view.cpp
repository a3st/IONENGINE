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

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { ptr.heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + ptr.offset * device->rtv_descriptor_offset_ };
    device->device_->CreateRenderTargetView(texture->resource_.Get(), &view_desc, cpu_handle);
    return ptr;
}

TextureView::TextureView() {

}

TextureView::TextureView(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc) {

    TextureFlags flags = texture->GetFlags();
    if(flags & TextureFlags::kRenderTarget) {
        ptr_ = CreateRenderTarget(device, pool, texture, desc);
    }
}

TextureView::TextureView(TextureView&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    std::swap(texture_, rhs.texture_);
    std::swap(ptr_, rhs.ptr_);
    std::swap(view_desc_, rhs.view_desc_);
}

TextureView& TextureView::operator=(TextureView&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    std::swap(texture_, rhs.texture_);
    std::swap(ptr_, rhs.ptr_);
    std::swap(view_desc_, rhs.view_desc_);
    return *this;
}