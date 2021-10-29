// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

const size_t kMemoryPoolDefaultHeapSize = 268435456;
const size_t kMemoryPoolDefaultBlockSize = 1048576;

class MemoryHeap {

friend class MemoryPool;
friend class Texture;
friend class Buffer;

public:

    MemoryHeap(Device* device, const uint64_t align, const MemoryType type, const MemoryFlags flags);
    MemoryHeap(const MemoryHeap&) = delete;
    MemoryHeap(MemoryHeap&&) = delete;

    MemoryHeap& operator=(const MemoryHeap&) = delete;
    MemoryHeap& operator=(MemoryHeap&&) = delete;

    inline size_t GetHeapSize() const { return heap_size_; }
    inline uint64_t GetBlockCount() const { return block_count_; }

private:

    ComPtr<ID3D12Heap> heap_;
    size_t heap_size_;
    uint64_t block_count_;
    std::vector<uint8_t> blocks_;
    uint64_t offset_;
};

struct MemoryAllocInfo {

    MemoryHeap* heap;
    uint64_t offset;
    size_t size;
};

class MemoryPool {

public:

    MemoryPool(Device* device, const size_t size, const uint64_t align, const MemoryType type, const MemoryFlags flags);
    MemoryPool(const MemoryPool&) = delete;
    MemoryPool(MemoryPool&&) = delete;

    MemoryPool& operator=(const MemoryPool&) = delete;
    MemoryPool& operator=(MemoryPool&&) = delete;

    MemoryAllocInfo Allocate(const size_t size);
    void Deallocate(const MemoryAllocInfo& alloc_info);

    inline MemoryType GetType() const { return type_; }
    inline MemoryFlags GetFlags() const { return flags_; }

private:

    inline size_t AlignedBlockSize(const size_t size) const {

        return size < kMemoryPoolDefaultBlockSize ? 
            kMemoryPoolDefaultBlockSize : (size % kMemoryPoolDefaultBlockSize) > 0 ?
                (size / kMemoryPoolDefaultBlockSize + 1) * kMemoryPoolDefaultBlockSize : size;
    }

    MemoryType type_;
    MemoryFlags flags_;
    std::vector<std::unique_ptr<MemoryHeap>> heaps_;
};

}