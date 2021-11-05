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
    Device* device, DescriptorPool* pool, 
    Buffer* buffer, 
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

BufferView::~BufferView() {

    if(pool_) {
        pool_->Deallocate(alloc_info_);
    }
}

BufferView::BufferView(BufferView&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    std::swap(buffer_, rhs.buffer_);

    if(buffer_->GetFlags() & BufferFlags::kVertexBuffer) {
        std::swap(vertex_view_desc_, rhs.vertex_view_desc_);
    } else if(buffer_->GetFlags() & BufferFlags::kIndexBuffer) {
        std::swap(index_view_desc_, rhs.index_view_desc_);
    }

    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(index_format_, rhs.index_format_);
    std::swap(stride_, rhs.stride_);
}

BufferView& BufferView::operator=(BufferView&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    std::swap(buffer_, rhs.buffer_);

    if(buffer_->GetFlags() & BufferFlags::kVertexBuffer) {
        std::swap(vertex_view_desc_, rhs.vertex_view_desc_);
    } else if(buffer_->GetFlags() & BufferFlags::kIndexBuffer) {
        std::swap(index_view_desc_, rhs.index_view_desc_);
    }

    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(index_format_, rhs.index_format_);
    std::swap(stride_, rhs.stride_);
    return *this;
}