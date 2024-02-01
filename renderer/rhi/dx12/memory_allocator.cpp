// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "core/exception.hpp"
#include "memory_allocator.hpp"
#include "precompiled.h"
#include "utils.hpp"

namespace ionengine::renderer::rhi
{
    DX12MemoryAllocator::DX12MemoryAllocator(ID3D12Device1* device, size_t const block_size, size_t const chunk_size,
                                             MemoryAllocatorUsage const usage)
        : device(device), block_size(block_size), chunk_size(chunk_size), usage(usage)
    {
        assert((chunk_size % block_size == 0) && "Chunk size must be completely divisible by block size");
        assert((chunk_size < DX12_MEMORY_ALLOCATOR_CHUNK_MAX_SIZE) &&
               "Chunk size must be less than DX12_MEMORY_ALLOCATOR_CHUNK_MAX_SIZE");

        chunks.try_emplace(D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT);
        chunks.try_emplace(D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
        chunks.try_emplace(D3D12_SMALL_MSAA_RESOURCE_PLACEMENT_ALIGNMENT);
        chunks.try_emplace(D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT);
    }

    auto DX12MemoryAllocator::allocate(D3D12_RESOURCE_DESC const& resource_desc) -> MemoryAllocation
    {
        auto allocation_info = device->GetResourceAllocationInfo(0, 1, &resource_desc);
        size_t const size = (allocation_info.SizeInBytes + (block_size - 1)) & ~(block_size - 1);

        std::lock_guard lock(mutex);

        assert((chunks.find(allocation_info.Alignment) != chunks.end()) &&
               "Required heap not found when allocating memory");

        auto allocation = MemoryAllocation{};

        for (auto& chunk : chunks[allocation_info.Alignment])
        {
            if (size > chunk.size - chunk.offset)
            {
                continue;
            }

            uint64_t alloc_start = chunk.offset;
            size_t alloc_size = 0;
            bool success = false;

            for (uint64_t i = chunk.offset; i < chunk.size; i += block_size)
            {
                if (alloc_size >= allocation_info.SizeInBytes)
                {
                    success = true;
                    break;
                }

                if (chunk.free[i / block_size] == 1)
                {
                    alloc_size += block_size;
                }
                else
                {
                    alloc_start = i + block_size;
                    alloc_size = 0;
                }
            }

            if (success)
            {
                std::fill(chunk.free.begin() + (alloc_start / block_size),
                          chunk.free.begin() + (alloc_start / block_size) + (alloc_size / block_size), 0);
                chunk.offset += alloc_size;

                allocation = {.heap = chunk.heap.get(),
                              .alignment = allocation_info.Alignment,
                              .offset = alloc_start,
                              .size = alloc_size};
                break;
            }
        }

        if (allocation.size == 0)
        {
            create_chunk(allocation_info.Alignment);

            auto& chunk = chunks[allocation_info.Alignment].back();
            uint64_t const alloc_start = chunk.offset;
            size_t const alloc_size = size;

            std::fill(chunk.free.begin() + (alloc_start / block_size),
                      chunk.free.begin() + (alloc_start / block_size) + (alloc_size / block_size), 0);
            chunk.offset += alloc_size;

            allocation = {.heap = chunk.heap.get(),
                          .alignment = allocation_info.Alignment,
                          .offset = alloc_start,
                          .size = alloc_size};
        }
        return allocation;
    }

    auto DX12MemoryAllocator::deallocate(MemoryAllocation const& allocation) -> void
    {
        std::lock_guard lock(mutex);

        assert((chunks.find(allocation.alignment) != chunks.end()) &&
               "Required heap not found when deallocating descriptor");
        assert((ptr_chunks.find((uintptr_t)allocation.heap) != ptr_chunks.end()) &&
               "Required chunk not found when deallocating descriptor");

        uint32_t const chunk_index = ptr_chunks[(uintptr_t)allocation.heap];
        auto& chunk = chunks[allocation.alignment][chunk_index];

        std::fill(chunk.free.begin() + (allocation.offset / block_size),
                  chunk.free.begin() + (allocation.offset / block_size) + (allocation.size / block_size), 1);
        chunk.offset = allocation.offset;
    }

    auto DX12MemoryAllocator::reset() -> void
    {
        assert(false && "DX12MemoryAllocator doesn't support reset method");
    }

    auto DX12MemoryAllocator::create_chunk(uint64_t const alignment) -> void
    {
        D3D12_HEAP_TYPE heap_type;
        if (usage == MemoryAllocatorUsage::Upload)
        {
            heap_type = D3D12_HEAP_TYPE_UPLOAD;
        }
        else if (usage == MemoryAllocatorUsage::Default)
        {
            heap_type = D3D12_HEAP_TYPE_DEFAULT;
        }

        auto d3d12_heap_desc = D3D12_HEAP_DESC{};
        d3d12_heap_desc.Alignment = alignment;
        d3d12_heap_desc.SizeInBytes = chunk_size;
        d3d12_heap_desc.Properties.Type = heap_type;

        winrt::com_ptr<ID3D12Heap> heap;
        THROW_IF_FAILED(device->CreateHeap(&d3d12_heap_desc, __uuidof(ID3D12Heap), heap.put_void()));

        auto chunk = Chunk{.heap = heap, .offset = 0, .size = chunk_size};
        chunk.free.resize(chunk_size / block_size, 1);
        chunks[alignment].emplace_back(std::move(chunk));

        ptr_chunks.emplace((uintptr_t)heap.get(), static_cast<uint32_t>(chunks[alignment].size() - 1));
    }
} // namespace ionengine::renderer::rhi