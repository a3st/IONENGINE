// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/backend/d3d12x.h>

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::backend;
using namespace ionengine::renderer::backend::d3d12;

HRESULT STDMETHODCALLTYPE IUnknownImpl::QueryInterface(REFIID riid, void** ppvObject) {
    if (ppvObject == NULL) {
        return E_POINTER;
    }

    if (riid == IID_IUnknown) {
        ++m_RefCount;
        *ppvObject = this;
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE IUnknownImpl::AddRef() {
    return ++m_RefCount;
}

ULONG STDMETHODCALLTYPE IUnknownImpl::Release() {
    const uint32_t newRefCount = --m_RefCount;
    if (newRefCount == 0) {
        ReleaseThis();
    }
    return newRefCount;
}

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

HRESULT DescriptorPool::allocate(DescriptorAllocation** allocation) {
    HRESULT result = E_OUTOFMEMORY;

    for(size_t i = 0; i < _heaps.size(); ++i) {
        detail::DescriptorHeap& heap = _heaps.at(i);

        if(heap.indices.empty()) {
            continue;
        }

        uint32_t const offset = heap.indices.front();
        heap.indices.pop_front();

        *allocation = &_allocations[i * heap.size + offset];

        result = S_OK;
        break;
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

    uint32_t const heap_size = 256;
    uint32_t const heap_count = std::max<uint32_t>(1, size / heap_size);

    _heaps.resize(heap_count);
    _allocations = new DescriptorPoolAllocation[heap_count * heap_size];

    for(uint32_t i = 0; i < heap_count; ++i) {
        auto pool_heap = detail::DescriptorHeap {};

        auto descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        descriptor_heap_desc.NumDescriptors = heap_size;
        descriptor_heap_desc.Type = heap_type;
        descriptor_heap_desc.Flags = heap_flags;

        result = device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(pool_heap.heap.GetAddressOf()));

        _heaps[i] = std::move(pool_heap);

        for(uint32_t j = 0; j < heap_size; ++j) {
            auto& cur_allocation = _allocations[i * heap_size + j];
            cur_allocation._heap = &_heaps[i];
            cur_allocation._offset = j;
            cur_allocation._descriptor_size = _descriptor_size;

            _heaps[i].indices.emplace_back(j);
        }
    }
    return result;
}

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
    // Not implemented because Descriptor Range Allocator is linear and reset occurs for all range allocator
}

HRESULT DescriptorRange::allocate(D3D12_DESCRIPTOR_RANGE const& descriptor_range, DescriptorAllocation** allocation) {
    HRESULT result = E_OUTOFMEMORY;

    uint32_t next_arena_block_index;
    uint32_t cur_arena_block_index = _arena_block_index.load(std::memory_order_acquire);

    do {
        next_arena_block_index = (cur_arena_block_index + 1) % static_cast<uint32_t>(_arena_block_ranges.size());
    }
    while(!_arena_block_index.compare_exchange_weak(cur_arena_block_index, next_arena_block_index, std::memory_order_acq_rel, std::memory_order_acquire));

    auto& arena_block = _arena_block_ranges.at(cur_arena_block_index);
    auto& cur_allocation = _allocations[cur_arena_block_index * arena_block.size + arena_block.offset];
    
    cur_allocation._heap = _heap;
    cur_allocation._descriptor_size = _descriptor_size;
    cur_allocation._offset = arena_block.begin + arena_block.offset;

    if(arena_block.offset + descriptor_range.NumDescriptors <= arena_block.size) {
        *allocation = &cur_allocation;
        arena_block.offset += descriptor_range.NumDescriptors;
        result = S_OK;
    }
    return result;
}

void DescriptorRange::reset() {
    _arena_block_index.store(0, std::memory_order_acquire);
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
    _descriptor_size = device->GetDescriptorHandleIncrementSize(descriptor_heap->GetDesc().Type);

    _heap = descriptor_heap;
    _offset = offset;
    _size = size;

    uint32_t const arena_range_count = std::thread::hardware_concurrency() > 16 ? 16 : std::thread::hardware_concurrency();

    _arena_block_ranges.resize(arena_range_count);
    _allocations = new DescriptorRangeAllocation[(size / arena_range_count) * arena_range_count];

    for(uint32_t i = 0; i < arena_range_count; ++i) {
        _arena_block_ranges[i].begin = offset + i * size / arena_range_count;
        _arena_block_ranges[i].size = size / arena_range_count;
    }
    return S_OK;
}

HRESULT ionengine::renderer::backend::d3d12::create_descriptor_pool(
    ID3D12Device4* const device, 
    D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
    D3D12_DESCRIPTOR_HEAP_FLAGS const heap_flags, 
    uint32_t const size,
    DescriptorPool** const descriptor_pool
) {
    (*descriptor_pool) = new DescriptorPool();
    return (*descriptor_pool)->initialize(device, heap_type, heap_flags, size);
}

HRESULT ionengine::renderer::backend::d3d12::create_descriptor_range(
    ID3D12Device4* const device, 
    ID3D12DescriptorHeap* const descriptor_heap,
    uint32_t const offset,
    uint32_t const size,
    DescriptorRange** const descriptor_range
) {
    *descriptor_range = new DescriptorRange();
    return (*descriptor_range)->initialize(device, descriptor_heap, offset, size);
}
