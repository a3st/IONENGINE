// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "descriptor_allocator.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

PoolDescriptorAllocator::PoolDescriptorAllocator(ID3D12Device1* device, std::span<DescriptorAllocatorHeap const> const heaps, bool const cpu_visible) {

    for(auto const& heap : heaps) {

        if(heap.heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) {
            assert(cpu_visible && "Descriptor allocator cannot contains RTV descriptors in CPU visible heap");
        }

        uint32_t remaining_size = heap.count;
        increment_sizes.try_emplace(heap.heap_type, device->GetDescriptorHandleIncrementSize(heap.heap_type));

        do {
            uint32_t alloc_size = 0;
            switch(heap.heap_type) {
                case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: {
                    alloc_size = remaining_size > DESCRIPTOR_HEAP_CBV_SRV_UAV_MAX ? DESCRIPTOR_HEAP_CBV_SRV_UAV_MAX : remaining_size;
                } break;
                case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: {
                    alloc_size = remaining_size > DESCRIPTOR_HEAP_RTV_MAX ? DESCRIPTOR_HEAP_RTV_MAX : remaining_size;
                } break;
                case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: {
                    alloc_size = remaining_size > DESCRIPTOR_HEAP_DSV_MAX ? DESCRIPTOR_HEAP_DSV_MAX : remaining_size;
                } break;
                case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: {
                    alloc_size = remaining_size > DESCRIPTOR_HEAP_SAMPLER_MAX ? DESCRIPTOR_HEAP_SAMPLER_MAX : remaining_size;
                } break;
            }

            auto d3d12_descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
            d3d12_descriptor_heap_desc.NumDescriptors = heap.count;
            d3d12_descriptor_heap_desc.Type = heap.heap_type;
            d3d12_descriptor_heap_desc.Flags = cpu_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

            winrt::com_ptr<ID3D12DescriptorHeap> descriptor_heap;

            THROW_IF_FAILED(device->CreateDescriptorHeap(
                &d3d12_descriptor_heap_desc, 
                __uuidof(ID3D12DescriptorHeap), 
                descriptor_heap.put_void())
            );

            auto chunk = Chunk {
                .heap = descriptor_heap,
                .offset = 0,
                .size = heap.count
            };
            chunk.free.resize(heap.count, 1);

            chunks.try_emplace(heap.heap_type);
            chunks[heap.heap_type].emplace_back(chunk);
            ptr_chunks.emplace((uintptr_t)descriptor_heap.get(), static_cast<uint32_t>(chunks[heap.heap_type].size() - 1));

            remaining_size -= alloc_size;
        } while(remaining_size > 0);
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

LinearDescriptorAllocator::LinearDescriptorAllocator(ID3D12Device1* device, std::span<DescriptorAllocatorHeap const> const ranges, bool const cpu_visible) {

}

auto LinearDescriptorAllocator::allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation {
    return {};
}

auto LinearDescriptorAllocator::deallocate(DescriptorAllocation const& allocation) -> void {

}

auto LinearDescriptorAllocator::reset() -> void {
    
}