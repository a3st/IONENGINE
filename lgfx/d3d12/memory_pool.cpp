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

MemoryHeap::MemoryHeap() {

}

MemoryHeap::MemoryHeap(Device* device, const uint64_t align, const MemoryType type, const MemoryFlags flags) {

    D3D12_HEAP_DESC heap_desc{};
    heap_desc.SizeInBytes = kMemoryPoolDefaultHeapSize;
    heap_desc.Properties.Type = ToD3D12HeapType(type);
    heap_desc.Flags = ToD3D12HeapFlags(flags);
    heap_desc.Alignment = align;

    THROW_IF_FAILED(device->device_->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(heap.GetAddressOf())));

    block_count = kMemoryPoolDefaultHeapSize / kMemoryPoolDefaultBlockSize;
    blocks.resize(block_count, 0x0);

    offset = 0;
}

MemoryHeap::MemoryHeap(MemoryHeap&& rhs) noexcept {

    heap.Swap(rhs.heap);
    std::swap(heap_size, rhs.heap_size);
    std::swap(block_count, rhs.block_count);
    std::swap(blocks, rhs.blocks);
    std::swap(offset, rhs.offset);
}

MemoryHeap& MemoryHeap::operator=(MemoryHeap&& rhs) noexcept {

    heap.Swap(rhs.heap);
    std::swap(heap_size, rhs.heap_size);
    std::swap(block_count, rhs.block_count);
    std::swap(blocks, rhs.blocks);
    std::swap(offset, rhs.offset);
    return *this;
}

MemoryPool::MemoryPool() {

}

MemoryPool::MemoryPool(Device* device, const size_t size, const uint64_t align, const MemoryType type, const MemoryFlags flags) :
    type_(type),
    flags_(flags) {

    size_t heap_count = size > kMemoryPoolDefaultHeapSize ? size / kMemoryPoolDefaultHeapSize : 1;
    heaps_.resize(heap_count);

    for(uint32_t i = 0; i < heap_count; ++i) {
        heaps_[i] = MemoryHeap(device, align, type, flags);
    }
}

MemoryPool::MemoryPool(MemoryPool&& rhs) noexcept {

    std::swap(type_, rhs.type_);
    std::swap(flags_, rhs.flags_);
    std::swap(heaps_, rhs.heaps_);
}

MemoryPool& MemoryPool::operator=(MemoryPool&& rhs) noexcept {

    std::swap(type_, rhs.type_);
    std::swap(flags_, rhs.flags_);
    std::swap(heaps_, rhs.heaps_);
    return *this;
}

MemoryPtr MemoryPool::Allocate(const size_t size) {

    MemoryPtr ptr{};
    size_t align_size = AlignedBlockSize(size);
    
    for(uint32_t i = 0; i < static_cast<uint32_t>(heaps_.size()); ++i) {
        
        if(heaps_[i].offset + align_size > heaps_[i].heap_size) {
            continue;
        } else {
            size_t alloc_size = 0;
            for(uint64_t j = 0; i < heaps_[i].block_count; ++j) {
                if(alloc_size == align_size) {
                    std::memset(heaps_[i].blocks.data() + ptr.offset / kMemoryPoolDefaultBlockSize, 0x1, sizeof(uint8_t) * align_size / kMemoryPoolDefaultBlockSize);
                    break;
                }

                if(alloc_size == 0) {
                    ptr.heap = &heaps_[i];
                    ptr.offset = j * kMemoryPoolDefaultBlockSize;
                }

                alloc_size = heaps_[i].blocks[j] == 0x0 ? alloc_size + kMemoryPoolDefaultBlockSize : alloc_size;
            }
            if(alloc_size != align_size) {
                ptr.heap = nullptr;
                ptr.offset = 0;
            }
        }
        if(ptr.heap) {
            break;
        }
    }
    return ptr;
}

void MemoryPool::Deallocate(MemoryPtr* ptr, const size_t size) {

    size_t align_size = AlignedBlockSize(size);
    std::memset(ptr->heap->blocks.data() + ptr->offset / kMemoryPoolDefaultBlockSize, 0x0, sizeof(uint8_t) * align_size / kMemoryPoolDefaultBlockSize);
    ptr->heap->offset = ptr->offset;
}