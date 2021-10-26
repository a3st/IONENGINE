// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "descriptor_pool.h"
#include "conversion.h"
#include "device.h"

using namespace lgfx;

DescriptorHeap::DescriptorHeap(Device* device, const DescriptorType type, const DescriptorFlags flags) {

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
    heap_desc.NumDescriptors = kDescriptorPoolDefaultHeapSize;
    heap_desc.Type = ToD3D12DescriptorHeapType(type);
    heap_desc.Flags = flags == DescriptorFlags::kShaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    THROW_IF_FAILED(device->device_->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(heap_.GetAddressOf())));

    heap_size_ = kDescriptorPoolDefaultHeapSize;
    descriptors_.resize(heap_size_, 0x0);

    offset_ = 0;
}

DescriptorPool::DescriptorPool(Device* device, const size_t size, const DescriptorType type, const DescriptorFlags flags) :
    type_(type),
    flags_(flags) {

    size_t heap_count = size > kDescriptorPoolDefaultHeapSize ? size / kDescriptorPoolDefaultHeapSize : 1;
    heaps_.resize(heap_count);

    for(uint32_t i = 0; i < heap_count; ++i) {
        heaps_[i] = std::make_unique<DescriptorHeap>(device, type, flags);
    }
}

DescriptorPtr DescriptorPool::Allocate() {

    DescriptorPtr ptr{};

    for(uint32_t i = 0; i < static_cast<uint32_t>(heaps_.size()); ++i) {
        if(heaps_[i]->offset_ > heaps_[i]->heap_size_) {
            continue;
        } else {
            for(uint32_t j = 0; j < heaps_[i]->heap_size_; ++j) {
                if(heaps_[i]->descriptors_[j] == 0x0) {
                    ptr.heap = heaps_[i].get();
                    ptr.offset = j;
                    heaps_[i]->descriptors_[j] = 0x1;
                    heaps_[i]->offset_ = j + 1;
                    break;
                }
            }
        }
        if(ptr.heap) {
            break;
        }
    }
    return ptr;
}

void DescriptorPool::Deallocate(DescriptorPtr* ptr) {

    ptr->heap->descriptors_[ptr->offset] = 0x0;
    ptr->heap->offset_ = ptr->offset;
}