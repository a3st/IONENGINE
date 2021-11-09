// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "buffer_view.h"
#include "device.h"
#include "buffer.h"
#include "conversion.h"

using namespace lgfx;

DescriptorAllocInfo BufferView::CreateConstantBuffer(Device* device) {

    DescriptorAllocInfo alloc_info = pool_->Allocate();

    D3D12_CONSTANT_BUFFER_VIEW_DESC view_desc{};
    view_desc.BufferLocation = buffer_->resource_->GetGPUVirtualAddress();
    view_desc.SizeInBytes = static_cast<uint32_t>(buffer_->GetSize());

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device->srv_descriptor_offset_ };
    device->device_->CreateConstantBufferView(&view_desc, cpu_handle);
    return alloc_info;
}

D3D12_INDEX_BUFFER_VIEW BufferView::CreateIndexBuffer() {

    D3D12_INDEX_BUFFER_VIEW view_desc{};
    view_desc.BufferLocation = buffer_->resource_->GetGPUVirtualAddress();
    view_desc.SizeInBytes = static_cast<uint32_t>(buffer_->GetSize());
    view_desc.Format = ToDXGIFormat(index_format_);
    return view_desc;
}

D3D12_VERTEX_BUFFER_VIEW BufferView::CreateVertexBuffer() {

    D3D12_VERTEX_BUFFER_VIEW view_desc{};
    view_desc.BufferLocation = buffer_->resource_->GetGPUVirtualAddress();
    view_desc.SizeInBytes = static_cast<uint32_t>(buffer_->GetSize());
    view_desc.StrideInBytes = stride_;
    return view_desc;
}

BufferView::BufferView(
    Device* const device, DescriptorPool* const pool, 
    Buffer* const buffer, 
    const Format index_format,
    const uint32_t stride) : 
        pool_(pool), 
        buffer_(buffer), 
        index_format_(index_format), 
        stride_(stride) {

    BufferFlags flags = buffer->GetFlags();
    if(flags & BufferFlags::kConstantBuffer) {
        alloc_info_ = CreateConstantBuffer(device);
    } else if(flags & BufferFlags::kVertexBuffer) {
        vertex_view_desc_ = CreateVertexBuffer();
    } else if(flags & BufferFlags::kIndexBuffer) {
        index_view_desc_ = CreateIndexBuffer();
    }
}