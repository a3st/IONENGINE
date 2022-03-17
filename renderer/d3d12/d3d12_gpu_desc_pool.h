// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12x.h>
#include <renderer/d3d12/d3d12_desc_pool.h>

namespace ionengine::renderer::d3d12 {

class GPUDescriptorHeap {
public:

    GPUDescriptorHeap() = default;

    GPUDescriptorHeap(GPUDescriptorHeap&& other) noexcept {

        _heap = other._heap;
        _descriptor_size = other._descriptor_size;
        _count = other._count;
        _begin = other._begin;

        _arena_blocks = other._arena_blocks;
        
        _alloc_index.store(other._alloc_index.load());
    }

    GPUDescriptorHeap& operator=(GPUDescriptorHeap&& other) noexcept {

        _heap = other._heap;
        _descriptor_size = other._descriptor_size;
        _count = other._count;
        _begin = other._begin;

        _arena_blocks = other._arena_blocks;
        
        _alloc_index.store(other._alloc_index.load());
        return *this;
    }

    GPUDescriptorHeap(ID3D12DescriptorHeap* const heap, uint32_t const descriptor_size, uint32_t const offset, uint32_t const count) : 
        _heap(heap), _begin(offset), _count(count) {

        uint32_t const arena_count = std::thread::hardware_concurrency();

        _arena_blocks.resize(arena_count);

        for(size_t i = 0; i < _arena_blocks.size(); ++i) {

            _arena_blocks[i].begin = offset * static_cast<uint32_t>(i) + count / arena_count;
            _arena_blocks[i].count = count / arena_count;
        }
    }

    DescriptorAllocInfo allocate(D3D12_DESCRIPTOR_RANGE const& descriptor_range) {

        uint32_t cur_alloc_index = _alloc_index.load();

        while(!_alloc_index.compare_exchange_strong(
            cur_alloc_index, 
            cur_alloc_index + 1 % static_cast<uint32_t>(_arena_blocks.size())
        )) { }

        auto alloc_info = DescriptorAllocInfo {};
        alloc_info.descriptor_size = _descriptor_size;

        if(_arena_blocks[cur_alloc_index].offset + descriptor_range.NumDescriptors < _arena_blocks[cur_alloc_index].count) {

            alloc_info.heap = _heap;
            alloc_info.offset = _arena_blocks[cur_alloc_index].offset;
            
            _arena_blocks[cur_alloc_index].offset += descriptor_range.NumDescriptors;
        } else {
            std::cerr << "[Error] GPUDescriptorHeap: Unable to allocate descriptors from the pool" << std::endl;
        }
        return alloc_info;
    }

    void reset() {

        _alloc_index.store(0);

        for(size_t i = 0; i < _arena_blocks.size(); ++i) {

            _arena_blocks[i].offset = 0;
        }
    }

    ID3D12DescriptorHeap* const get_heap() const { return _heap; }

private:

    struct ArenaBlock {
        uint32_t begin;
        uint32_t count;
        uint32_t offset;
    };

    ID3D12DescriptorHeap* _heap;
    uint32_t _descriptor_size{0};
    uint32_t _count{0};
    uint32_t _begin{0};

    std::vector<ArenaBlock> _arena_blocks;

    std::atomic<uint32_t> _alloc_index{0};
};

template<D3D12_DESCRIPTOR_HEAP_TYPE HeapType, size_t HeapSize>
class GPUDescriptorPool {
public:

    GPUDescriptorPool() = default;

    GPUDescriptorPool(GPUDescriptorPool const&) = delete;

    GPUDescriptorPool(GPUDescriptorPool&&) noexcept = delete;

    GPUDescriptorPool& operator=(GPUDescriptorPool const&) = delete;

    GPUDescriptorPool& operator=(GPUDescriptorPool&&) noexcept = delete;

    GPUDescriptorHeap* allocate(ID3D12Device4* const device, uint32_t const count) {

        if(!_is_initialized) {

            create_heap(device);
            _descriptor_size = device->GetDescriptorHandleIncrementSize(HeapType);
            _is_initialized = true;
        }

        if(_offset + count > HeapSize) {
            std::cerr << "[Error] GPUDescriptorHeap: Unable to allocate descriptor heap from the pool" << std::endl;
            return nullptr;
        }

        _alloc_heaps.push_back(GPUDescriptorHeap(_heap.Get(), _descriptor_size, _offset, count));
        _offset += count;
        return &_alloc_heaps.back();
    }

    void reset() {
        
        _alloc_heaps.clear();
        _offset = 0;
    }

private:

    bool _is_initialized{false};

    ComPtr<ID3D12DescriptorHeap> _heap;
    uint32_t _descriptor_size{0};
    uint32_t _offset{0};

    std::list<GPUDescriptorHeap> _alloc_heaps;

    void create_heap(ID3D12Device4* device) {

        auto heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        heap_desc.NumDescriptors = HeapSize;
        heap_desc.Type = HeapType;
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(_heap.GetAddressOf())));
    }
};

}
