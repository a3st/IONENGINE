// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/allocator.hpp"
#include "renderer/rhi/buffer.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine {

namespace renderer {

namespace rhi {

struct MemoryAllocation {
    ID3D12Heap* heap;
    uint64_t alignment;
    uint64_t offset;
    size_t size;
};

class DX12MemoryAllocator : public MemoryAllocator {
public:

    DX12MemoryAllocator(ID3D12Device1* device, size_t const block_size, size_t const chunk_size, BufferUsageFlags const flags);

    auto allocate(D3D12_RESOURCE_DESC const& resource_desc) -> MemoryAllocation;

    auto deallocate(MemoryAllocation const& allocation) -> void;

    auto reset() -> void;

private:

    struct Chunk {
        winrt::com_ptr<ID3D12Heap> heap;
        std::vector<uint8_t> free;
        uint64_t offset;
        size_t size;
    };

    ID3D12Device1* device;
    std::unordered_map<uint64_t, std::vector<Chunk>> chunks;
    std::unordered_map<uintptr_t, uint32_t> ptr_chunks;
    BufferUsageFlags flags;
    size_t block_size;
    size_t chunk_size;

    auto create_chunk(uint64_t const alignment) -> void;
};

}

}

}