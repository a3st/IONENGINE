// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <renderer/impl/descriptor_d3d12.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

HRESULT DescriptorAllocation::QueryInterface(REFIID riid, void** ppvObject) {
    if (ppvObject == nullptr) {
        return E_POINTER;
    }

    if (riid == IID_IUnknown) {
        ++_ref_count;
        *ppvObject = this;
        return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG DescriptorAllocation::AddRef() { return ++_ref_count; }

ULONG DescriptorAllocation::Release() {
    uint32_t const ref_count = --_ref_count;
    if (ref_count == 0) {
        ReleaseThis();
    }
    return ref_count;
}

void DescriptorAllocation::ReleaseThis() {
    if(_pPool) {
        _pPool->Free(this); 
    }
}

void DescriptorAllocation::Initialize(DescriptorPool* pPool,
                                      uint32_t const descriptorIndex,
                                      uint32_t const descriptorSize,
                                      uint32_t const descriptorCount) {
    _pPool = pPool;
    _descriptorIndex = descriptorIndex;
    _descriptorSize = descriptorSize;
    _descriptorCount = descriptorCount;
}

void DescriptorAllocation::Deinitialize() {
    _pPool = nullptr;
    _descriptorIndex = std::numeric_limits<uint32_t>::max();
    _descriptorSize = 0;
}

HRESULT DescriptorPool::QueryInterface(REFIID riid, void** ppvObject) {
    if (ppvObject == nullptr) {
        return E_POINTER;
    }

    if (riid == IID_IUnknown) {
        ++_ref_count;
        *ppvObject = this;
        return S_OK;
    }

    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG DescriptorPool::AddRef() { return ++_ref_count; }

ULONG DescriptorPool::Release() {
    uint32_t const ref_count = --_ref_count;
    if (ref_count == 0) {
        ReleaseThis();
    }
    return ref_count;
}

void DescriptorPool::ReleaseThis() { delete this; }

HRESULT DescriptorBlockPool::Allocate(uint32_t const size,
                                      DescriptorAllocation** ppAllocation) {
    assert(size == 1 && "DescriptorBlockPool can allocate only 1 descriptor");
                                    
    HRESULT result = E_FAIL;
    uint32_t index = 0;
    DescriptorAllocation* allocation = nullptr;
    {
        std::lock_guard lock(_mutex);

        if (_free.empty()) {
            result = E_OUTOFMEMORY;
        } else {
            index = _free.back();
            _free.pop_back();

            allocation = _allocations.back();
            _allocations.pop_back();
        }
    }
    allocation->Initialize(this, index, _descriptorSize, size);

    (*ppAllocation) = allocation;
    result = S_OK;

    return result;
}

void DescriptorBlockPool::SetCurrentFrameIndex(uint32_t const frameIndex) {}

void DescriptorBlockPool::Free(DescriptorAllocation* pAllocation) {
    std::lock_guard lock(_mutex);
    _free.emplace_back(pAllocation->_descriptorIndex);
    pAllocation->Deinitialize();
    _allocations.emplace_back(pAllocation);
}

void DescriptorBlockPool::Reset() {}

HRESULT ionengine::renderer::CreateDescriptorPool(
    ID3D12Device4* const pDevice, D3D12_DESCRIPTOR_HEAP_TYPE const heapType,
    uint32_t const heapSize, bool const shaderVisible,
    DescriptorPoolFlags const poolFlags,
    DescriptorPool** const ppDescriptorPool) {
    HRESULT result = E_FAIL;

    // Initialize class
    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
        .Type = heapType,
        .Flags = (shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
                                : D3D12_DESCRIPTOR_HEAP_FLAG_NONE)};

    uint32_t const descriptorSize = pDevice->GetDescriptorHandleIncrementSize(heapType); 

    if (poolFlags & DescriptorPoolFlags::Free) {
        descriptor_heap_desc.NumDescriptors = heapSize;

        auto pool = new DescriptorBlockPool();

        // Initialize class members
        pool->_free.resize(heapSize);
        std::iota(pool->_free.begin(), pool->_free.end(), 0);
        pool->_allocations.resize(heapSize);
        for(auto& allocation : pool->_allocations) {
            allocation = new DescriptorAllocation();
        }
        pool->_descriptorSize = descriptorSize;

        result = pDevice->CreateDescriptorHeap(
            &descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap),
            reinterpret_cast<void**>(pool->_heap.GetAddressOf()));

        (*ppDescriptorPool) = pool;

    } else {
        descriptor_heap_desc.NumDescriptors =
            heapSize * DESCRIPTOR_POOL_THREAD_COUNT;

        //auto pool = new DescriptorLinearPool();

        result = S_OK;

        //(*ppDescriptorPool) = pool;
    }

    return result;
}
