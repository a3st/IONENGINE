// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DDesciptorPool {
public:

    struct Key {
        ViewType view_type;
        bool shader_visible;

        bool operator<(const Key& rhs) const {
            return std::tie(view_type, shader_visible) < std::tie(rhs.view_type, rhs.shader_visible);
        }
    };

    D3DDesciptorPool(
        ID3D12Device4* d3d12_device, 
        const uint32 default_srv_count, 
        const uint32 default_rtv_count, 
        const uint32 default_dsv_count, 
        const uint32 default_sampler_count
    ) : 
        m_d3d12_device(d3d12_device), 
        m_default_srv_count(default_srv_count), 
        m_default_rtv_count(default_rtv_count),
        m_default_dsv_count(default_dsv_count),
        m_default_sampler_count(default_sampler_count) {

    }

    [[nodiscard]] D3DDescriptorPtr allocate(const ViewType view_type, const bool shader_visible) {

        Key key = {
            view_type,
            shader_visible
        };

        D3DDescriptorPtr ptr{};

        auto it_heap = m_descriptor_heaps.find(key);
        if(it_heap != m_descriptor_heaps.end()) {

            for(auto& heap : it_heap->second) {

                if(heap.offset == heap.heap_size) {
                    continue;
                } else {
                    uint32 alloc_offset = heap.offset + m_d3d12_device->GetDescriptorHandleIncrementSize(d3d12_descriptor_heap_type_to_gfx_enum(view_type));
                    ptr.cpu_handle = D3D12_CPU_DESCRIPTOR_HANDLE { 
                        heap.d3d12_heap->GetCPUDescriptorHandleForHeapStart() + alloc_offset;
                    };
                    ptr.gpu_handle =
                }
            }

        }
        return ptr;
    }

    void deallocate() {

    }

private:

    winrt::com_ptr<ID3D12DescriptorHeap> create_heap(const ViewType view_type, const uint32 count, const bool shader_visible) {
        winrt::com_ptr<ID3D12DescriptorHeap> d3d12_heap;

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
        heap_desc.Type = d3d12_descriptor_heap_type_to_gfx_enum(view_type);
        heap_desc.NumDescriptors = count;
        heap_desc.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        THROW_IF_FAILED(m_d3d12_device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), d3d12_heap.put_void()));
        return d3d12_heap;
    }

    ID3D12Device4* m_d3d12_device;

    std::map<Key, std::list<D3DDescriptorHeap>> m_descriptor_heaps;

    uint32 m_default_srv_count;
    uint32 m_default_rtv_count;
    uint32 m_default_dsv_count;
    uint32 m_default_sampler_count;
};

class D3DDescriptorAllocatorWrapper {
public:

    static void initialize(ID3D12Device4* d3d12_device) {

    }

private:


};

}