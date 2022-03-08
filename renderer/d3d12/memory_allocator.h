// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12_shared.h>

namespace ionengine::renderer::d3d12 {

struct MemoryHeap {
    size_t heap_size;
    size_t block_size;
    ComPtr<ID3D12Heap> heap;
    std::vector<uint8_t> blocks;
    uint64_t offset;
};

struct MemoryAllocInfo {
    MemoryHeap* _heap;
    size_t _size;
    uint64_t _offset;

    ID3D12Heap* heap() const { return _heap->heap.Get(); }
    uint64_t offset() const { return _offset; }
};

class MemoryAllocator {
public:

    MemoryAllocator() = default;

    MemoryAllocator(MemoryAllocator const&) = delete;
    
    MemoryAllocator(MemoryAllocator&&) = default;

    MemoryAllocator& operator=(MemoryAllocator const&) = delete;

    MemoryAllocator& operator=(MemoryAllocator&&) noexcept = default;

    MemoryAllocInfo allocate(ID3D12Device4* device, D3D12_HEAP_TYPE const heap_type, size_t const size);

    void deallocate(MemoryAllocInfo const& alloc_info);

    void reset();

private:

    using MemoryPool = std::pair<std::list<MemoryHeap>, size_t>;

    enum class HeapTypeIndex : uint16_t {
        Default = 1,
        Upload = 2,
        Readback = 3,
        Custom = 4
    };

    std::array<MemoryPool, 4> _pools;

    enum class BlockSize : size_t {
        _256kb = 256 * 1024,
        _128kb = 128 * 1024,
        _64kb = 64 * 1024
    };

    enum class HeapSize : size_t {
        _256mb = 256 * 1024 * 1024,
        _128mb = 128 * 1024 * 1024,
        _64mb = 64 * 1024 * 1024,
        _32mb = 32 * 1024 * 1024,
        _16mb = 16 * 1024 * 1024
    };

    void create_memory_pool(ID3D12Device4* const device, D3D12_HEAP_TYPE const heap_type);

    MemoryAllocInfo get_free_block(ID3D12Device4* const device, D3D12_HEAP_TYPE const heap_type, size_t const size);
};

}
