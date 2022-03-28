// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/d3d12/d3d12_descriptor_range.h>

using namespace ionengine::renderer::backend::d3d12;

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorRangeAllocation::cpu_handle(uint32_t const index) const {

    return D3D12_CPU_DESCRIPTOR_HANDLE { 
        .ptr = _heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _descriptor_size * // Device descriptor size
            _offset + index // Allocation offset
    };
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorRangeAllocation::gpu_handle(uint32_t const index) const {
        
    return D3D12_GPU_DESCRIPTOR_HANDLE { 
        .ptr = _heap->GetGPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _descriptor_size * // Device descriptor size
            _offset + index // Allocation offset
    };
}

void DescriptorRangeAllocation::ReleaseThis() {
    
}

DescriptorRangeAllocation::DescriptorRangeAllocation() {
    
}

HRESULT DescriptorRange::allocate(D3D12_DESCRIPTOR_RANGE const& descriptor_range, DescriptorAllocation** allocation) {

    HRESULT result = E_OUTOFMEMORY;

    uint32_t cur_arena_block_index = _arena_block_index.load();

    while(true) {
        uint32_t const next_arena_block_index = cur_arena_block_index + 1 % static_cast<uint32_t>(_arena_block_ranges.size());
        if(_arena_block_index.compare_exchange_strong(cur_arena_block_index, next_arena_block_index)) {
            break;
        }
    }

    auto& cur_allocation = _allocations[cur_arena_block_index * MAX_ALLOCATION_COUNT + _arena_block_ranges[cur_arena_block_index].offset];
    
    cur_allocation._heap = _heap;
    cur_allocation._descriptor_size = _descriptor_size;
    cur_allocation._offset = _arena_block_ranges[cur_arena_block_index].offset;

    if(cur_allocation._offset + descriptor_range.NumDescriptors < _arena_block_ranges[cur_arena_block_index].size) {
        _arena_block_ranges[cur_arena_block_index].offset += descriptor_range.NumDescriptors;
        result = S_OK;

        *allocation = &cur_allocation;
    }

    return result;
}

void DescriptorRange::reset() {

    _arena_block_index.store(0);

    for(auto& range : _arena_block_ranges) {
        range.offset = 0;
    }
}

ID3D12DescriptorHeap* DescriptorRange::heap() const {

    return _heap;
}

void DescriptorRange::ReleaseThis() {

    delete[] _allocations;
    delete this;
}

HRESULT DescriptorRange::initialize(
    ID3D12Device4* const device, 
    ID3D12DescriptorHeap* const descriptor_heap,
    uint32_t const offset,
    uint32_t const size
) {

    _heap = descriptor_heap;
    _offset = offset;
    _size = size;

    uint32_t const range_count = std::thread::hardware_concurrency();

    _arena_block_ranges.resize(range_count);
    _allocations = new DescriptorRangeAllocation[range_count * MAX_ALLOCATION_COUNT];

    for(uint32_t i = 0; i < range_count; ++i) {
        auto& range = _arena_block_ranges[i];
        range.begin = offset * i + size / range_count;
        range.size = size / range_count;
    }

    return S_OK;
}

DescriptorRange::DescriptorRange() {

}

namespace ionengine::renderer::backend::d3d12 {

HRESULT create_descriptor_range(
    ID3D12Device4* const device, 
    ID3D12DescriptorHeap* const descriptor_heap,
    uint32_t const offset,
    uint32_t const size,
    DescriptorRange** const descriptor_range
) {

    *descriptor_range = new DescriptorRange();
    return (*descriptor_range)->initialize(device, descriptor_heap, offset, size);
}

}
