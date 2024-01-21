// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "allocator.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

DX12MemoryAllocator::DX12MemoryAllocator(
    ID3D12Device1* device, 
    size_t const block_size, 
    size_t const chunk_size, 
    BufferUsageFlags const flags
) : 
    device(device),
    block_size(block_size),
    chunk_size(chunk_size)
{
    chunks.try_emplace(D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT);
    chunks.try_emplace(D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT);
    chunks.try_emplace(D3D12_SMALL_MSAA_RESOURCE_PLACEMENT_ALIGNMENT);
    chunks.try_emplace(D3D12_DEFAULT_MSAA_RESOURCE_PLACEMENT_ALIGNMENT);
}

auto DX12MemoryAllocator::allocate(D3D12_RESOURCE_DESC const& resource_desc) -> MemoryAllocation {

    auto allocation_info = device->GetResourceAllocationInfo(0, 1, &resource_desc);

    assert((chunks.find(allocation_info.Alignment) != chunks.end()) && "Required heap not found when allocating memory");

    auto allocation = MemoryAllocation {};

    for(auto& chunk : chunks[allocation_info.Alignment]) {
        if(allocation_info.SizeInBytes > chunk.size - chunk.offset) {
            continue;
        }

        uint64_t start = chunk.offset;
        size_t alloc_size = 0;
        bool success = false;

        for(uint64_t i = chunk.offset; i < chunk.size; i += block_size) {
            if(alloc_size >= allocation_info.SizeInBytes) {
                success = true;
                break;
            }

            if(chunk.free[i / block_size] == 1) {
                alloc_size += block_size;
            } else {
                start = chunk.offset + block_size;
                alloc_size = 0;
            }
        }

        if(success) {
            std::fill(chunk.free.begin() + start / block_size, chunk.free.begin() + start / block_size + alloc_size / block_size, 0);

            allocation = {
                .heap = chunk.heap.get(),
                .alignment = allocation_info.Alignment,
                .offset = start,
                .size = alloc_size
            };
            break;
        }
    }

    if(!allocation.heap) {
        create_chunk(allocation_info.Alignment);

        auto& chunk = chunks[allocation_info.Alignment].back();
        uint64_t start = chunk.offset;
        size_t alloc_size = (allocation_info.Alignment + (chunk_size - 1)) & ~(chunk_size - 1);

        std::fill(chunk.free.begin() + start / block_size, chunk.free.begin() + start / block_size + alloc_size / block_size, 0);

        allocation = {
            .heap = chunk.heap.get(),
            .alignment = allocation_info.Alignment,
            .offset = start,
            .size = alloc_size
        };
    }

    return allocation;
}

auto DX12MemoryAllocator::deallocate(MemoryAllocation const& allocation) -> void {

    assert((chunks.find(allocation.alignment) != chunks.end()) && "Required heap not found when deallocating descriptor");

    uint32_t const chunk_index = ptr_chunks[(uintptr_t)allocation.heap];
    auto& chunk = chunks[allocation.alignment][chunk_index];
    chunk.offset = allocation.offset;
    std::fill(chunk.free.begin() + allocation.offset / block_size, chunk.free.begin() + allocation.offset / block_size + allocation.size / block_size, 1);
}

auto DX12MemoryAllocator::reset() -> void {

    assert(false && "DX12MemoryAllocator doesn't support reset method");
}

auto DX12MemoryAllocator::create_chunk(uint64_t const alignment) -> void {

    assert((chunk_size % block_size == 0) && "Chunk size must be completely divisible by block size");

    D3D12_HEAP_TYPE heap_type;
    if(flags & BufferUsage::MapWrite) {
        heap_type = D3D12_HEAP_TYPE_UPLOAD;
    } else if(flags & BufferUsage::MapRead) {
        heap_type = D3D12_HEAP_TYPE_READBACK;
    } else {
        heap_type = D3D12_HEAP_TYPE_DEFAULT;
    }

    auto d3d12_heap_desc = D3D12_HEAP_DESC {};
    d3d12_heap_desc.Alignment = alignment;
    d3d12_heap_desc.SizeInBytes = chunk_size;

    winrt::com_ptr<ID3D12Heap> heap;
    THROW_IF_FAILED(device->CreateHeap(&d3d12_heap_desc, __uuidof(ID3D12Heap), heap.put_void()));

    auto chunk = Chunk {
        .heap = heap,
        .offset = 0,
        .size = chunk_size
    };
    chunk.free.resize(chunk_size / block_size, 1);
    chunks[alignment].emplace_back(chunk);
    ptr_chunks.emplace((uintptr_t)heap.get(), static_cast<uint32_t>(chunks[heap_type].size() - 1));
}
