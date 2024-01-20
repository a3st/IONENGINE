// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "descriptor_allocator.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

PoolDescriptorAllocator::PoolDescriptorAllocator(ID3D12Device1* device, bool const cpu_visible) : device(device), cpu_visible(cpu_visible) {

    std::vector<D3D12_DESCRIPTOR_HEAP_TYPE> heap_types;
    if(cpu_visible) {
        chunks.try_emplace(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        chunks.try_emplace(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        chunks.try_emplace(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        chunks.try_emplace(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    } else {
        chunks.try_emplace(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        chunks.try_emplace(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    }
}

auto PoolDescriptorAllocator::allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation {

    assert((chunks.find(heap_type) != chunks.end()) && "Required heap not found when allocating descriptor");

    DescriptorAllocation allocation;

    for(auto& chunk : chunks[heap_type]) {
        if(size > chunk.size - chunk.offset) {
            continue;
        }

        uint32_t start = chunk.offset;
        uint32_t alloc_size = 0;
        bool success = false;

        for(uint32_t i = chunk.offset; i < chunk.size; ++i) {
            if(alloc_size == size) {
                success = true;
                break;
            }

            if(chunk.free[i] == 1) {
                alloc_size++;
            } else {
                start = i + 1;
                alloc_size = 0;
            }
        }
        
        if(success) {
            std::fill(chunk.free.begin() + start, chunk.free.begin() + start + alloc_size, 0);

            allocation = {
                .heap = chunk.heap.get(),
                .heap_type = heap_type,
                .increment_size = increment_sizes[heap_type],
                .offset = start,
                .size = size
            };
            break;
        }
    }

    if(!allocation.heap) {
        create_chunk(heap_type, cpu_visible);

        auto& chunk = chunks[heap_type].back();
        uint32_t start = chunk.offset;
        uint32_t alloc_size = size;

        std::fill(chunk.free.begin() + start, chunk.free.begin() + start + alloc_size, 0);

        allocation = {
            .heap = chunk.heap.get(),
            .heap_type = heap_type,
            .increment_size = increment_sizes[heap_type],
            .offset = start,
            .size = size
        };
    }

    return allocation;
}

auto PoolDescriptorAllocator::deallocate(DescriptorAllocation const& allocation) -> void {

    assert((chunks.find(allocation.heap_type) != chunks.end()) && "Required heap not found when deallocating descriptor");

    uint32_t const chunk_index = ptr_chunks[(uintptr_t)allocation.heap];
    auto& chunk = chunks[allocation.heap_type][chunk_index];
    chunk.offset = allocation.offset;
    std::fill(chunk.free.begin() + allocation.offset, chunk.free.begin() + allocation.offset + allocation.size, 1);
}

auto PoolDescriptorAllocator::reset() -> void {
    
    assert(false && "PoolDescriptorAllocator doesn't support reset method");
}

auto PoolDescriptorAllocator::create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const cpu_visible) -> void {
	increment_sizes.try_emplace(heap_type, device->GetDescriptorHandleIncrementSize(heap_type));

	uint32_t alloc_size = 0;
	switch (heap_type) {
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: {
            alloc_size = DESCRIPTOR_HEAP_CBV_SRV_UAV_MAX;
        } break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: {
            alloc_size = DESCRIPTOR_HEAP_RTV_MAX;
        } break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: {
            alloc_size = DESCRIPTOR_HEAP_DSV_MAX;
        } break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: {
            alloc_size = DESCRIPTOR_HEAP_SAMPLER_MAX;
        } break;
	}

	auto d3d12_descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
	d3d12_descriptor_heap_desc.NumDescriptors = alloc_size;
	d3d12_descriptor_heap_desc.Type = heap_type;
	d3d12_descriptor_heap_desc.Flags = cpu_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	winrt::com_ptr<ID3D12DescriptorHeap> descriptor_heap;
	THROW_IF_FAILED(device->CreateDescriptorHeap(&d3d12_descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap), descriptor_heap.put_void()));

	auto chunk = Chunk {
        .heap = descriptor_heap, 
        .offset = 0, 
        .size = alloc_size
    };
	chunk.free.resize(alloc_size, 1);
	chunks[heap_type].emplace_back(chunk);
	ptr_chunks.emplace((uintptr_t)descriptor_heap.get(), static_cast<uint32_t>(chunks[heap_type].size() - 1));
}

LinearDescriptorAllocator::LinearDescriptorAllocator(ID3D12Device1* device, std::span<DescriptorAllocatorHeap const> const ranges, bool const cpu_visible) {

}

auto LinearDescriptorAllocator::allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation {
    return {};
}

auto LinearDescriptorAllocator::deallocate(DescriptorAllocation const& allocation) -> void {

}

auto LinearDescriptorAllocator::reset() -> void {
    
}