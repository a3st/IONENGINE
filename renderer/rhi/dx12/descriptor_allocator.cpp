// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "core/exception.hpp"
#include "descriptor_allocator.hpp"
#include "precompiled.h"
#include "utils.hpp"

namespace ionengine::renderer::rhi
{
    DescriptorAllocator::DescriptorAllocator(ID3D12Device1* device) : device(device)
    {
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    }

    auto DescriptorAllocator::allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size)
        -> DescriptorAllocation
    {
        assert((chunks.find(heap_type) != chunks.end()) && "Required heap not found when allocating descriptor");

        auto allocation = DescriptorAllocation{};
        auto& chunk = chunks[heap_type];

        if (size > chunk.size - chunk.offset)
        {
            throw core::Exception("Required heap does not contain free descriptors");
        }

        uint32_t alloc_start = chunk.offset;
        uint32_t alloc_size = 0;
        bool success = false;

        for (uint32_t const i : std::views::iota(chunk.offset, chunk.size))
        {
            if (alloc_size == size)
            {
                success = true;
                break;
            }

            if (chunk.free[i] == 1)
            {
                alloc_size++;
            }
            else
            {
                alloc_start = i + 1;
                alloc_size = 0;
            }
        }

        if (success)
        {
            std::fill(chunk.free.begin() + alloc_start, chunk.free.begin() + alloc_start + alloc_size, 0);
            chunk.offset += alloc_size;

            allocation = {.heap = chunk.heap.get(),
                          .heap_type = heap_type,
                          .increment_size = device->GetDescriptorHandleIncrementSize(heap_type),
                          .offset = alloc_start,
                          .size = size};
        }
        else
        {
            throw core::Exception("Required heap does not contain free descriptors");
        }
        return allocation;
    }

    auto DescriptorAllocator::deallocate(DescriptorAllocation const& allocation) -> void
    {
        assert((chunks.find(allocation.heap_type) != chunks.end()) &&
               "Required heap not found when deallocating descriptor");

        auto& chunk = chunks[allocation.heap_type];
        std::fill(chunk.free.begin() + allocation.offset, chunk.free.begin() + allocation.offset + allocation.size, 1);
        chunk.offset = allocation.offset;
    }

    auto DescriptorAllocator::create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type) -> void
    {
        uint32_t alloc_size = 0;
        D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        switch (heap_type)
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_CBV_SRV_UAV_MAX;
                flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_RTV_MAX;
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_DSV_MAX;
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: {
                alloc_size = DX12_DESCRIPTOR_ALLOCATOR_SAMPLER_MAX;
                flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                break;
            }
        }

        auto d3d12_descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC{};
        d3d12_descriptor_heap_desc.NumDescriptors = alloc_size;
        d3d12_descriptor_heap_desc.Type = heap_type;
        d3d12_descriptor_heap_desc.Flags = flags;

        winrt::com_ptr<ID3D12DescriptorHeap> descriptor_heap;
        THROW_IF_FAILED(device->CreateDescriptorHeap(&d3d12_descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap),
                                                     descriptor_heap.put_void()));

        auto chunk = Chunk{.heap = descriptor_heap, .offset = 0, .size = alloc_size};
        chunk.free.resize(alloc_size, 1);
        chunks.emplace(heap_type, chunk);
    }
} // namespace ionengine::renderer::rhi