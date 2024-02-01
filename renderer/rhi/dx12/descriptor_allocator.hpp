// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::renderer::rhi
{
    struct DescriptorAllocation
    {
        ID3D12DescriptorHeap* heap;
        D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
        uint32_t increment_size;
        uint32_t offset;
        uint32_t size;

        auto cpu_handle(uint32_t const index = 0) const -> D3D12_CPU_DESCRIPTOR_HANDLE
        {
            return {.ptr = heap->GetCPUDescriptorHandleForHeapStart().ptr + increment_size * offset + index};
        }

        auto gpu_handle(uint32_t const index = 0) const -> D3D12_GPU_DESCRIPTOR_HANDLE
        {
            return {.ptr = heap->GetGPUDescriptorHandleForHeapStart().ptr + increment_size * offset + index};
        }
    };

    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_RTV_MAX = 128;
    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_CBV_SRV_UAV_MAX = 16 * 1024;
    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_SAMPLER_MAX = 128;
    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_DSV_MAX = 32;

    class DescriptorAllocator : public core::ref_counted_object
    {
      public:
        DescriptorAllocator(ID3D12Device1* device);

        auto allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation;

        auto deallocate(DescriptorAllocation const& allocation) -> void;

        auto get_heap(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type) -> ID3D12DescriptorHeap*
        {
            return chunks[heap_type].heap.get();
        }

      private:
        struct Chunk
        {
            winrt::com_ptr<ID3D12DescriptorHeap> heap;
            std::vector<uint8_t> free;
            uint32_t offset;
            uint32_t size;
        };

        std::mutex mutex;
        ID3D12Device1* device;
        std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, Chunk> chunks;

        auto create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type) -> void;
    };
} // namespace ionengine::renderer::rhi