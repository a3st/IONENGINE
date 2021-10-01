// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<enum D3D12_DESCRIPTOR_HEAP_TYPE T, enum D3D12_DESCRIPTOR_HEAP_FLAGS F>
class D3DDesciptorPool {
public:

    D3DDesciptorPool(ID3D12Device4* d3d12_device, const uint32 default_descriptor_count) : m_d3d12_device(d3d12_device), m_default_descriptor_count(default_descriptor_count) {
        
    }

    [[nodiscard]] D3DDescriptorPtr allocate() {

        D3DDescriptorPtr ptr{};

        auto it_heap = m_descriptor_heaps.find(key);
        if(it_heap != m_descriptor_heaps.end()) {

            for(auto& heap : it_heap->second) {

                if(heap.offset == heap.heap_size) {
                    continue;
                } else {
                    uint32 alloc_offset = heap.offset + m_d3d12_device->GetDescriptorHandleIncrementSize(d3d12_descriptor_heap_type_to_gfx_enum(view_type));
                    ptr.cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { heap.d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + alloc_offset };
                    ptr.gpu_handle = D3D12_GPU_DESCRIPTOR_HANDLE { heap.d3d12_heap->GetGPUDescriptorHandleForHeapStart().ptr + alloc_offset };

                    heap.offset++;
                }
            }
        }
        if(!ptr.heap) {
            auto d3d12_heap = create_heap(m_default_descriptor_count);
        }
        return ptr;
    }

    void deallocate() {

    }

private:

    winrt::com_ptr<ID3D12DescriptorHeap> create_heap(const uint32 descriptor_count) {
        winrt::com_ptr<ID3D12DescriptorHeap> d3d12_heap;

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
        heap_desc.Type = m_heap_type;
        heap_desc.NumDescriptors = descriptor_count;
        heap_desc.Flags = m_heap_flags;

        THROW_IF_FAILED(m_d3d12_device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), d3d12_heap.put_void()));
        return d3d12_heap;
    }

    ID3D12Device4* m_d3d12_device;

    std::list<D3DDescriptorHeap> m_descriptor_heaps;

    uint32 m_default_descriptor_count;

    const D3D12_DESCRIPTOR_HEAP_TYPE m_heap_type = T;
    const D3D12_DESCRIPTOR_HEAP_FLAGS m_heap_flags = F;
};

class D3DDescriptorAllocatorWrapper {
public:

    static void initialize(ID3D12Device4* d3d12_device) {

    }

private:


};

}