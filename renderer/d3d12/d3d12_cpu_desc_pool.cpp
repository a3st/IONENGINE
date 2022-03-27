// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12_cpu_desc_pool.h>

using namespace ionengine::renderer::backend::d3d12;

namespace ionengine::renderer::backend::d3d12 {

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

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocation::cpu_handle(uint32_t const index) const {

    return D3D12_CPU_DESCRIPTOR_HANDLE { 
        .ptr = _heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _descriptor_size * // Device descriptor size
            _offset + index // Allocation offset
    };
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocation::gpu_handle(uint32_t const index = 0) const {
        
    return D3D12_GPU_DESCRIPTOR_HANDLE { 
        .ptr = _heap->GetGPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            _descriptor_size * // Device descriptor size
            _offset + index // Allocation offset
    };
}

struct DescriptorPoolImpl {

    struct PoolHeap {
        ComPtr<ID3D12DescriptorHeap> heap;
        uint32_t offset;
        std::list<DescriptorAllocation> allocations;
    };

    std::vector<PoolHeap> pool_heaps;

    uint32_t descriptor_size{0};

    void initialize(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size);

    HRESULT allocate(DescriptorAllocation** allocation);

    HRESULT deallocate(DescriptorAllocation* allocation);
};

}

void DescriptorPoolImpl_Deleter::operator()(DescriptorPoolImpl* ptr) const {

    delete ptr;
}

void DescriptorPoolImpl::initialize(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) {

    descriptor_size = device->GetDescriptorHandleIncrementSize(heap_type);

    uint32_t const heap_count = std::max<uint32_t>(1, size / CPUDescriptorPool::DESCRIPTOR_HEAP_SIZE);

    pool_heaps.resize(heap_count);

    for(uint32_t i = 0; i < heap_count; ++i) {

        auto pool_heap = DescriptorPoolImpl::PoolHeap {};

        auto descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        descriptor_heap_desc.NumDescriptors = CPUDescriptorPool::DESCRIPTOR_HEAP_SIZE;
        descriptor_heap_desc.Type = heap_type;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(pool_heap.heap.GetAddressOf())));

        for(uint32_t i = 0; i < CPUDescriptorPool::DESCRIPTOR_HEAP_SIZE; ++i) {

            pool_heap.allocations.emplace_back(this, i, descriptor_size);
        }

        pool_heaps[i] = std::move(pool_heap);
    }
}

HRESULT DescriptorPoolImpl::allocate(DescriptorAllocation** allocation) {

    HRESULT result = E_OUTOFMEMORY;

    for(auto& pool_heap : pool_heaps) {

        if(pool_heap.offset == CPUDescriptorPool::DESCRIPTOR_HEAP_SIZE) {
            continue;
        }

        *allocation = &pool_heap.allocations.front();
        pool_heap.allocations.pop_front();

        result = S_OK;
    }

    return result;
}

HRESULT DescriptorPoolImpl::deallocate(DescriptorAllocation* allocation) {


}

DescriptorAllocation::DescriptorAllocation(DescriptorPoolImpl* const pool, uint32_t const offset, uint32_t const descriptor_size) {

}

CPUDescriptorPool::CPUDescriptorPool(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) 
    : _impl(std::unique_ptr<DescriptorPoolImpl, DescriptorPoolImpl_Deleter>(new DescriptorPoolImpl())) {

    _impl->initialize(device, heap_type, size);
}

HRESULT CPUDescriptorPool::allocate(DescriptorAllocation** allocation) {

    return _impl->allocate(allocation);
}
    
HRESULT CPUDescriptorPool::deallocate(DescriptorAllocation* allocation) {

    return _impl->deallocate(allocation);
}