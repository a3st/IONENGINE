// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12_shared.h>

namespace ionengine::renderer::d3d12 {

struct MemoryAllocInfo {
    MemoryAllocator::MemoryHeap* _heap;
    size_t _size;
    uint64_t _offset;

    ID3D12Heap* heap() const { return _heap->heap.Get(); }
    uint64_t offset() const { return _offset; }
};

class MemoryAllocator {
public:

    struct MemoryHeap {
        ComPtr<ID3D12Heap> heap;
        std::vector<uint8_t> blocks;
        uint64_t offset;
    };

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
        Default,
        Upload,
        Readback,
        Custom
    };

    std::array<MemoryPool, 4> _pools;

    bool _is_allocated{false};

    void create_memory_pools(ID3D12Device4* device);
};

}