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
    view_desc.SizeInBytes = static_cast<uint32_t>(buffer_->resource_desc_.Width);

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device->srv_descriptor_offset_ };
    device->device_->CreateConstantBufferView(&view_desc, cpu_handle);
    return alloc_info;
}

D3D12_INDEX_BUFFER_VIEW BufferView::CreateIndexBuffer() {

    D3D12_INDEX_BUFFER_VIEW view_desc{};
    view_desc.BufferLocation = buffer_->resource_->GetGPUVirtualAddress();
    view_desc.SizeInBytes = static_cast<uint32_t>(buffer_->resource_desc_.Width);
    view_desc.Format = ToDXGIFormat(desc_.index_format);
    return view_desc;
}

D3D12_VERTEX_BUFFER_VIEW BufferView::CreateVertexBuffer() {

    D3D12_VERTEX_BUFFER_VIEW view_desc{};
    view_desc.BufferLocation = buffer_->resource_->GetGPUVirtualAddress();
    view_desc.SizeInBytes = static_cast<uint32_t>(buffer_->resource_desc_.Width);
    view_desc.StrideInBytes = desc_.stride;
    return view_desc;
}

BufferView::BufferView(Device* device, DescriptorPool* pool, Buffer* buffer, const BufferViewDesc& desc) :
    pool_(pool), buffer_(buffer), desc_(desc) {

    BufferFlags flags = buffer->GetDesc().flags;
    if(flags & BufferFlags::kConstantBuffer) {
        alloc_info_ = CreateConstantBuffer(device);
    } else if(flags & BufferFlags::kVertexBuffer) {
        vertex_view_desc = CreateVertexBuffer();
    } else if(flags & BufferFlags::kIndexBuffer) {
        index_view_desc = CreateIndexBuffer();
    }
}

BufferView::~BufferView() {

    if(pool_) {
        pool_->Deallocate(alloc_info_);
    }
}