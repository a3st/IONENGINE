// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <renderer/impl/pool_d3d12.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

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

HRESULT DescriptorPool::allocate(uint32_t const size) {

    
    {
        std::shared_lock lock(_mutex);

        auto it = _thread_data.find(std::this_thread::get_id());

        if (it != _thread_data.end()) {
            
        }
    }

    {
        std::unique_lock lock(_mutex);

        uint32_t const thread_index =
            static_cast<uint32_t>(_thread_data.size());

        ThreadPoolData thread_pool_data = {
            .begin = _heap->GetDesc().NumDescriptors * thread_index};

        if (_free_descriptors) {
            thread_pool_data.descriptors.resize(
                _heap->GetDesc().NumDescriptors);
            std::iota(thread_pool_data.descriptors.begin(),
                      thread_pool_data.descriptors.end(), 0);
        }

        _thread_data.insert(
            {std::this_thread::get_id(), std::move(thread_pool_data)});

        
    }
}

HRESULT ionengine::renderer::create_descriptor_pool(
    ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
    uint32_t const heap_size, bool const shader_visible,
    DescriptorPool** const descriptor_pool) {
    DescriptorPool* pool = new DescriptorPool();
    HRESULT result = E_FAIL;

    // Initialize class
    D3D12_DESCRIPTOR_HEAP_DESC descriptor_heap_desc = {
        .Type = heap_type,
        .NumDescriptors = heap_size * DESCRIPTOR_POOL_THREAD_COUNT,
        .Flags = (shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
                                 : D3D12_DESCRIPTOR_HEAP_FLAG_NONE)};

    pool->_free_descriptors = !shader_visible;

    result = device->CreateDescriptorHeap(
        &descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap),
        reinterpret_cast<void**>(pool->_heap.GetAddressOf()));

    (*descriptor_pool) = pool;

    return result;
}
