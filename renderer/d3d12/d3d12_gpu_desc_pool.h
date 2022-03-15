// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12x.h>

namespace ionengine::renderer::d3d12 {

struct DescriptorRangeAllocInfo {

    ID3D12DescriptorHeap* _heap;
    uint32_t _offset;

    ID3D12DescriptorHeap* heap() const { return _heap; }

    uint32_t offset() const { return _offset; }

    std::atomic<uint32_t> _alloc_count{0};

    uint32_t allocate(ID3D12Device4* device, D3D12_DESCRIPTOR_RANGE const& descriptor_range) {

        const uint32_t current_alloc = _alloc_count.load();
        _alloc_count.store(current_alloc + 1 % std::thread::hardware_concurrency());


    }

};

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, size_t HeapSize>
class GPUDescriptorPool {
public:

    GPUDescriptorPool();

    GPUDescriptorPool(GPUDescriptorPool const&) = delete;

    GPUDescriptorPool(GPUDescriptorPool&&) noexcept = delete;

    GPUDescriptorPool& operator=(GPUDescriptorPool const&) = delete;

    GPUDescriptorPool& operator=(GPUDescriptorPool&&) noexcept = delete;

    DescriptorRangeAllocInfo allocate(ID3D12Device4* const device, size_t const size) {

    }

    void deallocate(DescriptorRangeAllocInfo const& alloc_info) {

    }

    void reset(DescriptorRangeAllocInfo const& alloc_info) {
        
    }

private:

    bool _is_initialized{false};

    ComPtr<ID3D12DescriptorHeap> _heap;

    void create_heap(ID3D12Device4* device) {

        auto heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        heap_desc.NumDescriptors = HeapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV ? 4096 : 1024;
        heap_desc.Type = HeapType;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(_heap.GetAddressOf())));
    }
};

}
