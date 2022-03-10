// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/d3d12/memory_allocator.h>

using namespace ionengine::renderer::d3d12;

void MemoryAllocator::create_memory_pool(ID3D12Device4* const device, D3D12_HEAP_TYPE const heap_type) {

    auto create_heap = [&](ID3D12Device4* const device, D3D12_HEAP_TYPE const heap_type, size_t const size, ID3D12Heap** heap) -> HRESULT {
        auto heap_desc = D3D12_HEAP_DESC {};
        heap_desc.SizeInBytes = size;
        heap_desc.Properties.Type = heap_type;
        return device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(heap));
    };

    auto memory_heap = MemoryHeap {};

    size_t block_size;
    size_t heap_size;
    uint32_t heap_index = heap_type - 1;

    switch(heap_type) {
        case D3D12_HEAP_TYPE_DEFAULT: {

            HRESULT result;
            result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_256mb), memory_heap.heap.GetAddressOf());
            block_size = static_cast<size_t>(BlockSize::_256kb);
            heap_size = static_cast<size_t>(HeapSize::_256mb);
            if(result == E_OUTOFMEMORY) {
                result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_128mb), memory_heap.heap.GetAddressOf());
                block_size = static_cast<size_t>(BlockSize::_256kb);
                heap_size = static_cast<size_t>(HeapSize::_128mb);
            }
            if(result == E_OUTOFMEMORY) {
                result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_64mb), memory_heap.heap.GetAddressOf());
                block_size = static_cast<size_t>(BlockSize::_128kb);
                heap_size = static_cast<size_t>(HeapSize::_64mb);
            }
            THROW_IF_FAILED(result);
        } break;
        case D3D12_HEAP_TYPE_UPLOAD: {

            HRESULT result;
            result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_128mb), memory_heap.heap.GetAddressOf());
            block_size = static_cast<size_t>(BlockSize::_256kb);
            heap_size = static_cast<size_t>(HeapSize::_128mb);
            if(result == E_OUTOFMEMORY) {
                result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_64mb), memory_heap.heap.GetAddressOf());
                block_size = static_cast<size_t>(BlockSize::_128kb);
                heap_size = static_cast<size_t>(HeapSize::_64mb);
            }
            if(result == E_OUTOFMEMORY) {
                result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_32mb), memory_heap.heap.GetAddressOf());
                block_size = static_cast<size_t>(BlockSize::_64kb);
                heap_size = static_cast<size_t>(HeapSize::_32mb);
            }
            THROW_IF_FAILED(result);
        } break;
        case D3D12_HEAP_TYPE_READBACK: {

            HRESULT result;
            result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_64mb), memory_heap.heap.GetAddressOf());
            block_size = static_cast<size_t>(BlockSize::_128kb);
            heap_size = static_cast<size_t>(HeapSize::_64mb);
            if(result == E_OUTOFMEMORY) {
                result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_32mb), memory_heap.heap.GetAddressOf());
                block_size = static_cast<size_t>(BlockSize::_64kb);
                heap_size = static_cast<size_t>(HeapSize::_32mb);
            }
            if(result == E_OUTOFMEMORY) {
                result = create_heap(device, heap_type, static_cast<size_t>(HeapSize::_16mb), memory_heap.heap.GetAddressOf());
                block_size = static_cast<size_t>(BlockSize::_64kb);
                heap_size = static_cast<size_t>(HeapSize::_16mb);
            }
            THROW_IF_FAILED(result);
        } break;
        case D3D12_HEAP_TYPE_CUSTOM: {
            assert(false && "custom heap not supported yet");
        } break;
    }

    memory_heap.blocks.resize(heap_size / block_size);
    memory_heap.heap_size = heap_size;
    memory_heap.block_size = block_size;

    _pools[heap_index].first.emplace_back(memory_heap);
    _pools[heap_index].second += heap_size;
}

MemoryAllocInfo MemoryAllocator::get_free_block(ID3D12Device4* const device, D3D12_HEAP_TYPE const heap_type, size_t const size) {

    auto align_block_size = [&](size_t const block_size, size_t const size) -> size_t {
        return size < block_size ? block_size : (size % block_size) > 0 ? (size / block_size + 1) * block_size : size;
    };

    uint32_t heap_index = heap_type - 1;

    auto memory_alloc_info = MemoryAllocInfo {};

    for(auto& heap : _pools[heap_index].first) {
        size_t align_size = align_block_size(heap.block_size, size);
        if(heap.offset + align_size > heap.heap_size) {
            continue;
        } else {
            memory_alloc_info._heap = &heap;
            memory_alloc_info._size = align_size;

            size_t alloc_size = 0;
            for(size_t i = 0; i < heap.blocks.size(); ++i) {
                if(alloc_size == align_size) {
                    std::memset(heap.blocks.data() + memory_alloc_info._offset / heap.block_size, 0x1, sizeof(uint8_t) * alloc_size / heap.block_size);
                    heap.offset = memory_alloc_info._offset + alloc_size;
                    break;
                }
                if(alloc_size == 0) {
                    memory_alloc_info._offset = i * heap.block_size;
                }
                alloc_size = heap.blocks[i] == 0x0 ? alloc_size + heap.block_size : 0;
            }

            if(alloc_size != align_size) {
                memory_alloc_info._heap = nullptr;
            }
        }

        if(memory_alloc_info._heap && memory_alloc_info._size > 0) {
            break;
        }
    }

    if(!memory_alloc_info._heap) {

        create_memory_pool(device, heap_type);
        
        MemoryHeap& heap = _pools[heap_index].first.back();

        size_t align_size = align_block_size(heap.block_size, size);

        heap.offset = align_size;
        std::memset(heap.blocks.data(), 0x1, sizeof(uint8_t) * align_size / heap.block_size);
        
        memory_alloc_info._heap = &heap;
        memory_alloc_info._size = align_size;
        memory_alloc_info._offset = 0;
    }

    return memory_alloc_info;
}

MemoryAllocInfo MemoryAllocator::allocate(ID3D12Device4* const device, D3D12_HEAP_TYPE const heap_type, size_t const size) {

    return get_free_block(device, heap_type, size);
}

void MemoryAllocator::deallocate(MemoryAllocInfo const& alloc_info) {

    MemoryHeap& heap = *alloc_info._heap;

    heap.offset = alloc_info._offset;

    std::memset(heap.blocks.data() + alloc_info._offset / heap.block_size, 0x0, sizeof(uint8_t) * alloc_info._size / heap.block_size);
}

void MemoryAllocator::reset() {

    for(uint32_t i = 0; i < _pools.size(); ++i) {

        _pools[i].first.clear();
        _pools[i].second = 0;
    }
}
