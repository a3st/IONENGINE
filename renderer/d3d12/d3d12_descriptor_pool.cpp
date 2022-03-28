// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/d3d12/d3d12_descriptor_pool.h>

using namespace ionengine::renderer::backend::d3d12;

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorPoolAllocation::cpu_handle(uint32_t const index) const {

    return D3D12_CPU_DESCRIPTOR_HANDLE { 
        .ptr = _heap->heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _descriptor_size * // Device descriptor size
            _offset + index // Allocation offset
    };
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorPoolAllocation::gpu_handle(uint32_t const index) const {
        
    return D3D12_GPU_DESCRIPTOR_HANDLE { 
        .ptr = _heap->heap->GetGPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _descriptor_size * // Device descriptor size
            _offset + index // Allocation offset
    };
}

void DescriptorPoolAllocation::ReleaseThis() {
    
    _heap->indices.emplace_back(_offset);
}

DescriptorPoolAllocation::DescriptorPoolAllocation() {
    
}

HRESULT DescriptorPool::allocate(DescriptorAllocation** allocation) {

    HRESULT result = E_OUTOFMEMORY;

    for(size_t i = 0; i < _heaps.size(); ++i) {

        if(_heaps[i].offset == DescriptorPool::DESCRIPTOR_HEAP_SIZE) {
            continue;
        }

        uint32_t const offset = _heaps[i].indices.front();
        _heaps[i].indices.pop_front();

        *allocation = &_allocations[i * DescriptorPool::DESCRIPTOR_HEAP_SIZE + offset];

        result = S_OK;
    }

    return result;
}

void DescriptorPool::ReleaseThis() {

    delete[] _allocations;
    delete this;
}

HRESULT DescriptorPool::initialize(
    ID3D12Device4* const device, 
    D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, 
    D3D12_DESCRIPTOR_HEAP_FLAGS const heap_flags,
    uint32_t const size
) {
    
    HRESULT result = E_FAIL;

    _descriptor_size = device->GetDescriptorHandleIncrementSize(heap_type);

    uint32_t const heap_count = std::max<uint32_t>(1, size / DescriptorPool::DESCRIPTOR_HEAP_SIZE);

    _heaps.resize(heap_count);
    _allocations = new DescriptorPoolAllocation[heap_count * DescriptorPool::DESCRIPTOR_HEAP_SIZE];

    for(uint32_t i = 0; i < heap_count; ++i) {

        auto pool_heap = detail::DescriptorHeap {};

        auto descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        descriptor_heap_desc.NumDescriptors = DescriptorPool::DESCRIPTOR_HEAP_SIZE;
        descriptor_heap_desc.Type = heap_type;
        descriptor_heap_desc.Flags = heap_flags;

        result = device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(pool_heap.heap.GetAddressOf()));

        _heaps[i] = std::move(pool_heap);

        for(uint32_t j = 0; j < DescriptorPool::DESCRIPTOR_HEAP_SIZE; ++j) {
            _heaps[i].indices.emplace_back(j);
            
            auto& cur_allocation = _allocations[i * DescriptorPool::DESCRIPTOR_HEAP_SIZE + j];
            cur_allocation._heap = &_heaps[i];
            cur_allocation._offset = j;
            cur_allocation._descriptor_size = _descriptor_size;
        }
    }

    return result;
}

DescriptorPool::DescriptorPool() {
    
}

namespace ionengine::renderer::backend::d3d12 {

HRESULT create_descriptor_pool(
    ID3D12Device4* const device, 
    D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
    D3D12_DESCRIPTOR_HEAP_FLAGS const heap_flags, 
    uint32_t const size,
    DescriptorPool** const descriptor_pool
) {

    (*descriptor_pool) = new DescriptorPool();
    return (*descriptor_pool)->initialize(device, heap_type, heap_flags, size);
}

}
