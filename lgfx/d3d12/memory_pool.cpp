// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "memory_pool.h"
#include "conversion.h"
#include "device.h"

using namespace lgfx;

size_t MemoryPool::AlignedBlockSize(const size_t size) const {

    return size < kMemoryPoolDefaultBlockSize ? 
        kMemoryPoolDefaultBlockSize : (size % kMemoryPoolDefaultBlockSize) > 0 ?
            ((size / kMemoryPoolDefaultBlockSize) + 1) * kMemoryPoolDefaultBlockSize : size;
}

MemoryHeap::MemoryHeap(Device* device, const uint64_t align, const MemoryType type, const MemoryFlags flags) {

    D3D12_HEAP_DESC heap_desc{};
    heap_desc.SizeInBytes = kMemoryPoolDefaultHeapSize;
    heap_desc.Properties.Type = ToD3D12HeapType(type);
    heap_desc.Flags = ToD3D12HeapFlags(flags);
    heap_desc.Alignment = align;

    THROW_IF_FAILED(device->device_->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(heap_.GetAddressOf())));

    heap_size_ = kMemoryPoolDefaultHeapSize;

    block_count_ = kMemoryPoolDefaultHeapSize / kMemoryPoolDefaultBlockSize;
    blocks_.resize(block_count_, 0x0);

    offset_ = 0;
}

MemoryPool::MemoryPool(Device* device, const size_t size, const uint64_t align, const MemoryType type, const MemoryFlags flags) :
    type_(type),
    flags_(flags) {

    size_t heap_count = size > kMemoryPoolDefaultHeapSize ? size / kMemoryPoolDefaultHeapSize : 1;
    heaps_.resize(heap_count);

    for(uint32_t i = 0; i < heap_count; ++i) {
        heaps_[i] = std::make_unique<MemoryHeap>(device, align, type, flags);
    }
}

MemoryAllocInfo MemoryPool::Allocate(const size_t size) {

    MemoryAllocInfo alloc_info{};
    size_t align_size = AlignedBlockSize(size);

    for(uint32_t i = 0; i < static_cast<uint32_t>(heaps_.size()); ++i) {
        if(heaps_[i]->offset_ + align_size > heaps_[i]->heap_size_) {
            continue;
        } else {
            size_t alloc_size = 0;
            for(uint64_t j = 0; j < heaps_[i]->block_count_; ++j) {
                if(alloc_size == align_size) {
                    std::memset(heaps_[i]->blocks_.data() + alloc_info.offset / kMemoryPoolDefaultBlockSize, 0x1, sizeof(uint8_t) * align_size / kMemoryPoolDefaultBlockSize);
                    break;
                }

                if(alloc_size == 0) {
                    alloc_info.heap = heaps_[i].get();
                    alloc_info.offset = j * kMemoryPoolDefaultBlockSize;
                }

                alloc_size = heaps_[i]->blocks_[j] == 0x0 ? alloc_size + kMemoryPoolDefaultBlockSize : alloc_size;
            }
            if(alloc_size != align_size) {
                alloc_info.heap = nullptr;
                alloc_info.offset = 0;
            }
        }
        if(alloc_info.heap) {
            break;
        }
    }
    return alloc_info;
}

void MemoryPool::Deallocate(const MemoryAllocInfo& alloc_info, const size_t size) {

    size_t align_size = AlignedBlockSize(size);
    std::memset(alloc_info.heap->blocks_.data() + alloc_info.offset / kMemoryPoolDefaultBlockSize, 0x0, sizeof(uint8_t) * align_size / kMemoryPoolDefaultBlockSize);
    alloc_info.heap->offset_ = alloc_info.offset;
}
