// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12x.h>

namespace ionengine::renderer::d3d12 {

struct DescriptorAllocInfo {
    ID3D12DescriptorHeap* _heap;
    uint32_t _offset;

    ID3D12DescriptorHeap* heap() const { return _heap; }
    
    uint32_t offset() const { return _offset; }
};

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, size_t Size>
class DescriptorPool {
public:

    DescriptorPool() {

        fill_data();
    }

    DescriptorPool(DescriptorPool const&) = delete;

    DescriptorPool(DescriptorPool&&) noexcept = delete;

    DescriptorPool& operator=(DescriptorPool const&) = delete;

    DescriptorPool& operator=(DescriptorPool&&) noexcept = delete;

    DescriptorAllocInfo allocate(ID3D12Device4* device) {

        if(!_is_initialized) {
            create_heap(device);
            _is_initialized = true;
        }

        assert(!_free_indices.empty() && "The descriptor allocator is full");

        uint32_t const index = _free_indices.front();
        _free_indices.pop_front();

        auto alloc_info = DescriptorAllocInfo {};
        alloc_info._heap = _heap.Get();
        alloc_info._offset = index;
        return alloc_info;
    }
    
    void deallocate(DescriptorAllocInfo const& alloc_info) {

        _free_indices.emplace_back(alloc_info._offset);
    }

    void reset() {

        _free_indices.clear();

        fill_data();
    }

private:

    bool _is_initialized{false};

    ComPtr<ID3D12DescriptorHeap> _heap;

    std::list<uint32_t> _free_indices;

    void create_heap(ID3D12Device4* device) {

        auto heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        heap_desc.NumDescriptors = Size;
        heap_desc.Type = HeapType;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(_heap.GetAddressOf())));
    }

    void fill_data() {

        for(uint32_t i = 0; i < Size; ++i) {

            _free_indices.emplace_back(i);
        }
    }
};

}
