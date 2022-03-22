// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12x.h>

namespace ionengine::renderer::d3d12 {

struct DescriptorAllocInfo {
    ID3D12DescriptorHeap* heap;
    uint32_t offset;
    uint32_t descriptor_size;

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(uint32_t const index = 0) const {

        return D3D12_CPU_DESCRIPTOR_HANDLE { 
            heap->GetCPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            descriptor_size * // Device descriptor size
            offset + index // Allocation offset
        };
    }

    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(uint32_t const index = 0) const {
        
        return D3D12_GPU_DESCRIPTOR_HANDLE { 
            heap->GetGPUDescriptorHandleForHeapStart().ptr + // Base descriptor pointer
            descriptor_size * // Device descriptor size
            offset + index // Allocation offset
        };
    }
};

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, uint32_t HeapSize>
class CPUDescriptorPool {
public:

    CPUDescriptorPool() {

        divide_space_to_blocks();
    }

    CPUDescriptorPool(CPUDescriptorPool const&) = delete;

    CPUDescriptorPool(CPUDescriptorPool&& other) noexcept {

        _is_initialized = other._is_initialized;
        _heap.Swap(other._heap);
        _descriptor_size = other._descriptor_size;
        _offset = other._offset;
        _alloc_descriptors.swap(other._alloc_descriptors);
    }

    CPUDescriptorPool& operator=(CPUDescriptorPool const&) = delete;

    CPUDescriptorPool& operator=(CPUDescriptorPool&& other) noexcept {
        
        _is_initialized = other._is_initialized;
        _heap.Swap(other._heap);
        _descriptor_size = other._descriptor_size;
        _offset = other._offset;
        _alloc_descriptors.swap(other._alloc_descriptors);   
        return *this;
    }

    DescriptorAllocInfo allocate(ID3D12Device4* device) {

        if(!_is_initialized) {
            create_heap(device);
            _descriptor_size = device->GetDescriptorHandleIncrementSize(HeapType);
            _is_initialized = true;
        }

        auto alloc_info = DescriptorAllocInfo {};
        alloc_info.descriptor_size = _descriptor_size;

        if(_alloc_descriptors.empty()) {
            std::cerr << "[Error] CPUDescriptorPool: Unable to allocate descriptor from the pool" << std::endl;
            return alloc_info;
        }

        uint32_t const index = _alloc_descriptors.front();
        _alloc_descriptors.pop_front();
        
        alloc_info.heap = _heap.Get();
        alloc_info.offset = index;

        ++_offset;
        return alloc_info;
    }
    
    void deallocate(DescriptorAllocInfo const& alloc_info) {

        _alloc_descriptors.emplace_back(alloc_info.offset);
    }

    void reset() {

        _alloc_descriptors.clear();
        _offset = 0;

        divide_space_to_blocks();
    }

    uint32_t space() const { return HeapSize - _offset; }

private:

    bool _is_initialized{false};

    ComPtr<ID3D12DescriptorHeap> _heap;
    uint32_t _descriptor_size{0};
    uint32_t _offset{0};

    std::list<uint32_t> _alloc_descriptors;

    void create_heap(ID3D12Device4* device) {

        auto heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        heap_desc.NumDescriptors = HeapSize;
        heap_desc.Type = HeapType;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(_heap.GetAddressOf())));
    }

    void divide_space_to_blocks() {

        for(uint32_t i = 0; i < HeapSize; ++i) {

            _alloc_descriptors.emplace_back(i);
        }
    }
};

}
