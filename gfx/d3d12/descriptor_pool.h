// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<enum D3D12_DESCRIPTOR_HEAP_TYPE T>
class D3DDesciptorPool {
public:

    D3DDesciptorPool(ID3D12Device4* d3d12_device, const uint32 default_descriptor_count) : m_d3d12_device(d3d12_device), m_default_descriptor_count(default_descriptor_count) {
        
    }

    [[nodiscard]] D3DDescriptorPtr allocate() {

        D3DDescriptorPtr ptr{};

        for(auto& heap : m_descriptor_heaps) {

            if(heap.offset == heap.heap_size) {
                continue;
            } else {
                for(uint32 i = heap.offset; i < heap.descriptor_data.size(); ++i) {
                    if(heap.descriptor_data[i] == 0x1) {
                        continue;
                    }

                    ptr.heap = &heap;
                    ptr.offset = i;

                    heap.offset = i + 1;

                    std::cout << "descriptor pool free block found" << std::endl;
                }
            }
        }
        if(!ptr.heap) {
            auto d3d12_heap = create_heap(m_default_descriptor_count);
            auto& heap = m_descriptor_heaps.emplace_back(D3DDescriptorHeap { d3d12_heap, m_default_descriptor_count });

            heap.descriptor_data.resize(m_default_descriptor_count);
            std::memset(heap.descriptor_data.data() + heap.offset, 0x0, sizeof(uint8) * m_default_descriptor_count);

            ptr.heap = &heap;
            ptr.offset = heap.offset;
            
            heap.descriptor_data[heap.offset] = 0x1;
            heap.offset++;

            std::cout << "descriptor pool allocating new heap" << std::endl;
        }
        return ptr;
    }

    void deallocate(const D3DDescriptorPtr& ptr) {

        ptr.heap->descriptor_data[ptr.offset] = 0x0;
        ptr.heap->offset = ptr.offset;

        std::cout << "descriptor pool deallocate memory" << std::endl;
    }

private:

    winrt::com_ptr<ID3D12DescriptorHeap> create_heap(const uint32 descriptor_count) {
        winrt::com_ptr<ID3D12DescriptorHeap> d3d12_heap;

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
        heap_desc.Type = m_heap_type;
        heap_desc.NumDescriptors = descriptor_count;
        heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        THROW_IF_FAILED(m_d3d12_device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), d3d12_heap.put_void()));
        return d3d12_heap;
    }

    ID3D12Device4* m_d3d12_device;

    std::list<D3DDescriptorHeap> m_descriptor_heaps;

    uint32 m_default_descriptor_count;

    const D3D12_DESCRIPTOR_HEAP_TYPE m_heap_type = T;
};

class D3DDescriptorAllocatorWrapper {
public:

    static void initialize(ID3D12Device4* d3d12_device) {
        m_srv_pool = std::make_unique<D3DDesciptorPool<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>>(d3d12_device, 10);
    }

    [[nodiscard]] static D3DDescriptorPtr allocate(const ViewType view_type) {

        D3DDescriptorPtr ptr{};

        switch(view_type) {
            case ViewType::ConstantBuffer: ptr = m_srv_pool->allocate();
        }

        return ptr;
    }

    static void deallocate(const ViewType view_type, const D3DDescriptorPtr& ptr) {

        switch(view_type) {
            case ViewType::ConstantBuffer: m_srv_pool->deallocate(ptr);
        }
    }

private:

    inline static std::unique_ptr<D3DDesciptorPool<D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV>> m_srv_pool;
};

}