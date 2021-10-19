// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "descriptor_pool.h"
#include "conversion.h"
#include "device.h"

using namespace lgfx;

DescriptorHeap::DescriptorHeap() {

}

DescriptorHeap::DescriptorHeap(Device* device, const DescriptorType type, const DescriptorFlags flags) {

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
    heap_desc.NumDescriptors = kDescriptorPoolDefaultHeapSize;
    heap_desc.Type = ToD3D12DescriptorHeapType(type);
    heap_desc.Flags = flags == DescriptorFlags::kShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    THROW_IF_FAILED(device->device_->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&heap)));

    heap_size = kDescriptorPoolDefaultHeapSize;
    descriptors.resize(heap_size, 0x0);

    offset = 0;
}

DescriptorHeap::~DescriptorHeap() {
    heap->Release();
}

DescriptorPool::DescriptorPool() {

}

DescriptorPool::~DescriptorPool() {

}

DescriptorPool::DescriptorPool(Device* device, const size_t size, const DescriptorType type, const DescriptorFlags flags) :
    type_(type),
    flags_(flags) {

    size_t heap_count = size > kDescriptorPoolDefaultHeapSize ? size / kDescriptorPoolDefaultHeapSize : 1;
    heaps_.resize(heap_count);

    for(uint32_t i = 0; i < heap_count; ++i) {
        heaps_[i] = DescriptorHeap(device, type, flags);
    }
}

DescriptorPool::DescriptorPool(DescriptorPool&& rhs) noexcept {

    type_ = rhs.type_;
    flags_ = rhs.flags_;

    heaps_ = std::move(rhs.heaps_);
}

DescriptorPool& DescriptorPool::operator=(DescriptorPool&& rhs) noexcept {

    type_ = rhs.type_;
    flags_ = rhs.flags_;

    heaps_ = std::move(rhs.heaps_);
    return *this;
}

DescriptorPtr DescriptorPool::Allocate() {

    DescriptorPtr ptr{};

    for(uint32_t i = 0; i < static_cast<uint32_t>(heaps_.size()); ++i) {
        if(heaps_[i].offset > heaps_[i].heap_size) {
            continue;
        } else {
            for(uint64_t j = 0; i < heaps_[i].heap_size; ++j) {
                if(heaps_[i].descriptors[j] == 0x0) {
                    ptr.heap = &heaps_[i];
                    ptr.offset = i;
                    heaps_[i].descriptors[j] = 0x1;
                    heaps_[i].offset = i + 1;
                    break;
                }
            }
        }
        if(ptr.heap != nullptr) {
            break;
        }
    }
    return ptr;
}

void DescriptorPool::Deallocate(DescriptorPtr* ptr) {

    ptr->heap->descriptors[ptr->offset] = 0x0;
    ptr->heap->offset = ptr->offset;
}