// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/d3d12/memory_allocator.h>

using namespace ionengine::renderer::d3d12;

void MemoryAllocator::create_memory_pools(ID3D12Device4* device) {

    auto memory_heap = MemoryHeap {};

    auto heap_desc = D3D12_HEAP_DESC {};
    heap_desc.SizeInBytes = 256 * 1024 * 1024;
    heap_desc.Properties.Type = D3D12_HEAP_TYPE_DEFAULT;

    memory_heap.blocks.resize(heap_desc.SizeInBytes / 1048576);

    THROW_IF_FAILED(device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(memory_heap.heap.GetAddressOf())));

    _pools[static_cast<size_t>(HeapTypeIndex::Default)].first.emplace_back(memory_heap);
    _pools[static_cast<size_t>(HeapTypeIndex::Default)].second = 256 * 1024 * 1024;

    memory_heap = MemoryHeap {};

    heap_desc = D3D12_HEAP_DESC {};
    heap_desc.SizeInBytes = 128 * 1024 * 1024;
    heap_desc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;

    memory_heap.blocks.resize(heap_desc.SizeInBytes / 1048576);

    THROW_IF_FAILED(device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(memory_heap.heap.GetAddressOf())));

    _pools[static_cast<size_t>(HeapTypeIndex::Upload)].first.emplace_back(memory_heap);
    _pools[static_cast<size_t>(HeapTypeIndex::Upload)].second = 128 * 1024 * 1024;

    memory_heap = MemoryHeap {};

    heap_desc = D3D12_HEAP_DESC {};
    heap_desc.SizeInBytes = 64 * 1024 * 1024;
    heap_desc.Properties.Type = D3D12_HEAP_TYPE_READBACK;

    memory_heap.blocks.resize(heap_desc.SizeInBytes / 1048576);

    THROW_IF_FAILED(device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(memory_heap.heap.GetAddressOf())));

    _pools[static_cast<size_t>(HeapTypeIndex::Readback)].first.emplace_back(memory_heap);
    _pools[static_cast<size_t>(HeapTypeIndex::Readback)].second = 64 * 1024 * 1024;

    // TODO! Custom Heap not supported yet
}

MemoryAllocInfo MemoryAllocator::allocate(ID3D12Device4* device, D3D12_HEAP_TYPE const heap_type, size_t const size) {

    if(!_is_allocated) {
        create_memory_pools(device);
    }

    /*D3D12_HEAP_DESC heap_desc{};
    heap_desc.SizeInBytes = heap_size;
    heap_desc.Properties.Type = heap_type;

    ComPtr<ID3D12Heap> heap;
    THROW_IF_FAILED(device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), reinterpret_cast<void**>(heap.GetAddressOf())));

    _heaps.emplace_back(MemoryHeap { heap, std::vector<uint8_t>(heap_size / 1048576, 0x0), 0 });*/

    /*auto get_align_size = [&](size_t const size) -> size_t {
        return size < 1048576 ? 1048576 : (size % 1048576) > 0 ? (size / 1048576 + 1) * 1048576 : size;
    };

    auto memory_alloc_info = MemoryAllocInfo {};

    size_t align_size = get_align_size(size);

    for(auto& heap : _heaps) {
        if(heap.offset + align_size > _heap_size) {
            continue;
        } else {
            size_t alloc_size = 0;
            for(uint64_t i = 0; i < static_cast<uint64_t>(heap.blocks.size()); ++i) {
                if(alloc_size == align_size) {
                    std::memset(heap.blocks.data() + memory_alloc_info.offset / 1048576, 0x1, sizeof(uint8_t) * align_size / 1048576);
                    break;
                }

                if(alloc_size == 0) {
                    memory_alloc_info.heap = &heap;
                    memory_alloc_info.offset = i * 1048576;
                    memory_alloc_info.size = align_size;
                }

                alloc_size = heap.blocks[i] == 0x0 ? alloc_size + 1048576 : 0;
            }
            if(alloc_size != align_size) {
                memory_alloc_info.heap = nullptr;
                memory_alloc_info.offset = 0;
                memory_alloc_info.size = 0;
            }
        }
        if(memory_alloc_info.heap) {
            break;
        }
    }

    if(!memory_alloc_info.heap) {

    }

    return memory_alloc_info;*/
}

void MemoryAllocator::deallocate(MemoryAllocInfo const& alloc_info) {

    //std::memset(alloc_info.heap->blocks.data() + alloc_info.offset / 1048576, 0x0, sizeof(uint8_t) * alloc_info.size / 1048576);
    //--alloc_info.heap->offset;
}

void MemoryAllocator::reset() {
    
}