// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine {

namespace renderer {

namespace rhi {

struct DescriptorAllocation {
    ID3D12DescriptorHeap* heap;
    D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
    uint32_t increment_size;
    uint32_t offset;
    uint32_t size;

    auto cpu_handle(uint32_t const index = 0) -> D3D12_CPU_DESCRIPTOR_HANDLE {
        return {
            .ptr = heap->GetCPUDescriptorHandleForHeapStart().ptr + increment_size * offset + index
        };
    }

    auto gpu_handle(uint32_t const index = 0) -> D3D12_GPU_DESCRIPTOR_HANDLE {
        return {
            .ptr = heap->GetGPUDescriptorHandleForHeapStart().ptr + increment_size * offset + index
        };
    }
};

class DescriptorAllocator : public core::ref_counted_object {
public:

    virtual auto allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation = 0;

    virtual auto deallocate(DescriptorAllocation const& allocation) -> void = 0;

    virtual auto reset() -> void = 0;
};

struct DescriptorAllocatorHeap {
    D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
    uint32_t count;
};

class PoolDescriptorAllocator : public DescriptorAllocator {
public:

    PoolDescriptorAllocator(ID3D12Device1* device, bool const cpu_visible);

    auto allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation override;

    auto deallocate(DescriptorAllocation const& allocation) -> void override;

    auto reset() -> void override;

private:

    inline static uint32_t DESCRIPTOR_HEAP_RTV_MAX = 128;
    inline static uint32_t DESCRIPTOR_HEAP_CBV_SRV_UAV_MAX = 256;
    inline static uint32_t DESCRIPTOR_HEAP_SAMPLER_MAX = 256;
    inline static uint32_t DESCRIPTOR_HEAP_DSV_MAX = 32;

    std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, uint32_t> increment_sizes;

    struct Chunk {
        winrt::com_ptr<ID3D12DescriptorHeap> heap;
        std::vector<uint8_t> free;
        uint32_t offset;
        uint32_t size;
    };

    ID3D12Device1* device;
    std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, std::vector<Chunk>> chunks;
    std::unordered_map<uintptr_t, uint32_t> ptr_chunks;
    bool cpu_visible;

    auto create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const cpu_visible) -> void;
};

class LinearDescriptorAllocator : public DescriptorAllocator {
public:

    LinearDescriptorAllocator(ID3D12Device1* device, std::span<DescriptorAllocatorHeap const> const ranges, bool const cpu_visible);

    auto allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation override;

    auto deallocate(DescriptorAllocation const& allocation) -> void override;

    auto reset() -> void override;

private:


};

}

}

}