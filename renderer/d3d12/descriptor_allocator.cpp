// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/d3d12/descriptor_allocator.h>

using namespace ionengine::renderer::d3d12;

void DescriptorAllocator::create_descriptor_pool(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const shader_visible) {

    auto create_heap = [&](ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const count, bool const shader_visible, ID3D12DescriptorHeap** heap) -> HRESULT {
        auto heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        heap_desc.NumDescriptors = count;
        heap_desc.Type = heap_type;
        heap_desc.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        return device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(heap));
    };

    auto descriptor_heap = DescriptorHeap {};

    uint32_t heap_size;
    uint16_t heap_index;

    switch(heap_type) {
        case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: {
            HRESULT result;
            if(shader_visible) {
                result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_2048), shader_visible, descriptor_heap.heap.GetAddressOf());
                heap_size = static_cast<uint32_t>(HeapSize::_2048);
                heap_index = static_cast<uint16_t>(HeapTypeIndex::Sampler_Shader);
                if(result == E_OUTOFMEMORY) {
                    result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_1024), shader_visible, descriptor_heap.heap.GetAddressOf());
                    heap_size = static_cast<uint32_t>(HeapSize::_1024);
                }
            } else {
                result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_256), shader_visible, descriptor_heap.heap.GetAddressOf());
                heap_size = static_cast<uint32_t>(HeapSize::_256);
                heap_index = static_cast<uint16_t>(HeapTypeIndex::Sampler);
                if(result == E_OUTOFMEMORY) {
                    result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_128), shader_visible, descriptor_heap.heap.GetAddressOf());
                    heap_size = static_cast<uint32_t>(HeapSize::_128);
                }
            }
            THROW_IF_FAILED(result);
        } break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: {
            HRESULT result;
            if(shader_visible) {
                result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_4096), shader_visible, descriptor_heap.heap.GetAddressOf());
                heap_size = static_cast<uint32_t>(HeapSize::_4096);
                heap_index = static_cast<uint16_t>(HeapTypeIndex::SRV_CBV_UAV_Shader);
                if(result == E_OUTOFMEMORY) {
                    result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_2048), shader_visible, descriptor_heap.heap.GetAddressOf());
                    heap_size = static_cast<uint32_t>(HeapSize::_2048);
                }
                if(result == E_OUTOFMEMORY) {
                    result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_1024), shader_visible, descriptor_heap.heap.GetAddressOf());
                    heap_size = static_cast<uint32_t>(HeapSize::_1024);
                }
            } else {
                result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_256), shader_visible, descriptor_heap.heap.GetAddressOf());
                heap_size = static_cast<uint32_t>(HeapSize::_256);
                heap_index = static_cast<uint16_t>(HeapTypeIndex::SRV_CBV_UAV);
                if(result == E_OUTOFMEMORY) {
                    result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_128), shader_visible, descriptor_heap.heap.GetAddressOf());
                    heap_size = static_cast<uint32_t>(HeapSize::_128);
                }
            }
            THROW_IF_FAILED(result);
        } break;
        case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
        case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: {
            HRESULT result;
            result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_256), shader_visible, descriptor_heap.heap.GetAddressOf());
            heap_size = static_cast<uint32_t>(HeapSize::_256);
            heap_index = heap_type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV ? static_cast<uint16_t>(HeapTypeIndex::RTV) : static_cast<uint16_t>(HeapTypeIndex::DSV);
            if(result == E_OUTOFMEMORY) {
                result = create_heap(device, heap_type, static_cast<uint32_t>(HeapSize::_128), shader_visible, descriptor_heap.heap.GetAddressOf());
                heap_size = static_cast<uint32_t>(HeapSize::_128);
            }
            THROW_IF_FAILED(result);
        } break;
    }

    descriptor_heap.heap_size = heap_size;
    descriptor_heap.blocks.resize(heap_size);

    if(shader_visible && _pools[heap_index].first.size() > 0) {
        throw ionengine::Exception(u8"An error occurred while creating the descriptor heap. Shader visible heap should be 1 per type");
    }

    _pools[heap_index].first.emplace_back(descriptor_heap);
    _pools[heap_index].second += heap_size;
}

DescriptorAllocInfo DescriptorAllocator::get_free_block(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const shader_visible) {

    uint16_t heap_index = heap_type;
    if(shader_visible) {
        switch(heap_type) {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: heap_index = 4; break;
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: heap_index = 5; break;
        }
    }

    auto descriptor_alloc_info = DescriptorAllocInfo {};

    for(auto& heap : _pools[heap_index].first) {
        if(heap.offset + 1 > heap.heap_size) {     
            continue;
        } else {
            descriptor_alloc_info._heap = &heap;

            for(size_t i = 0; i < heap.blocks.size(); ++i) {
                if(heap.blocks[i] == 0x0) {
                    descriptor_alloc_info._offset = static_cast<uint32_t>(i);
                    heap.blocks[i] = 0x1;
                    ++heap.offset;
                    break;
                }
            }
        }

        if(descriptor_alloc_info._heap) {
            break;
        }
    }

    if(!descriptor_alloc_info._heap) {

        create_descriptor_pool(device, heap_type, shader_visible);

        DescriptorHeap& heap = _pools[heap_index].first.back();

        ++heap.offset;
        heap.blocks[0] = 0x1;

        descriptor_alloc_info._heap = &heap;
        descriptor_alloc_info._offset = 0;
    }

    return descriptor_alloc_info;
}

DescriptorAllocInfo DescriptorAllocator::allocate(ID3D12Device4* device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const shader_visible) {

    return get_free_block(device, heap_type, shader_visible);
}

void DescriptorAllocator::deallocate(DescriptorAllocInfo const& alloc_info) {

    DescriptorHeap& heap = *alloc_info._heap;

    heap.offset = alloc_info._offset;

    heap.blocks[alloc_info._offset] = 0x0;
}

void DescriptorAllocator::reset() {

    for(uint32_t i = 0; i < _pools.size(); ++i) {

        _pools[i].first.clear();
        _pools[i].second = 0;
    }
}
