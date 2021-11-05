// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "buffer.h"
#include "device.h"

using namespace lgfx;

Buffer::Buffer(Device* device, MemoryPool* pool, const size_t size, const BufferFlags flags) : pool_(pool), size_(size), flags_(flags) {

    D3D12_RESOURCE_STATES initial_state;
    D3D12_RESOURCE_DESC resource_desc{};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = size_;
    resource_desc.Height = 1;
    resource_desc.MipLevels = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    if(flags_ & BufferFlags::kConstantBuffer) {
        resource_desc.Width = (size_ + 255) & ~255;
    }

    switch(pool_->GetType()) {
        case MemoryType::kDefault: {
            initial_state = D3D12_RESOURCE_STATE_COMMON;
            break;
        }
        case MemoryType::kReadBack:
        case MemoryType::kUpload: {
            initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
            break;
        }
    }

    D3D12_RESOURCE_ALLOCATION_INFO alloc_info = device->device_->GetResourceAllocationInfo(0, 1, &resource_desc);
    alloc_info_ = pool_->Allocate(alloc_info.SizeInBytes + alloc_info.Alignment);
    
    THROW_IF_FAILED(device->device_->CreatePlacedResource(alloc_info_.heap->heap_.Get(), alloc_info_.offset, &resource_desc, initial_state, nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource_.GetAddressOf())));
}

Buffer::~Buffer() {

    if(pool_) {
        pool_->Deallocate(alloc_info_);
    }
}

std::byte* Buffer::Map() {

    std::byte* data;
    D3D12_RANGE range{};
    THROW_IF_FAILED(resource_->Map(0, &range, reinterpret_cast<void**>(&data)));
    return data;
}

void Buffer::Unmap() {
    
    D3D12_RANGE range{};
    resource_->Unmap(0, &range);
}

Buffer::Buffer(Buffer&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    resource_.Swap(rhs.resource_);
    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(size_, rhs.size_);
    std::swap(flags_, rhs.flags_);
}

Buffer& Buffer::operator=(Buffer&& rhs) noexcept {

    std::swap(pool_, rhs.pool_);
    resource_.Swap(rhs.resource_);
    std::swap(alloc_info_, rhs.alloc_info_);
    std::swap(size_, rhs.size_);
    std::swap(flags_, rhs.flags_);
    return *this;
}