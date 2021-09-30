// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DDesciptorPool {
public:

    struct Key {
        D3D12_DESCRIPTOR_HEAP_TYPE heap_type;

        bool operator<(const Key& rhs) const {
            return std::tie(heap_type) < std::tie(rhs.heap_type);
        }
    };

    D3DDesciptorPool(ID3D12Device4* d3d12_device, const usize size) : m_d3d12_device(d3d12_device) {

    }

    [[nodiscard]] D3DDescriptorPtr allocate(const ViewType view_type, const bool shader_visible) {


    }

    void deallocate() {

    }

private:

    winrt::com_ptr<ID3D12DescriptorHeap> create_heap(const ViewType view_type, const usize size, const bool shader_visible) {
        winrt::com_ptr<ID3D12DescriptorHeap> d3d12_heap;

        D3D12_DESCRIPTOR_HEAP_DESC heap_desc{};
        heap_desc.Type = d3d12_descriptor_heap_type_to_gfx_enum(view_type);
        heap_desc.NumDescriptors = static_cast<uint32>(size);
        heap_desc.Flags = shader_visible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        THROW_IF_FAILED(m_d3d12_device->CreateDescriptorHeap(&heap_desc, __uuidof(ID3D12DescriptorHeap), d3d12_heap.put_void()));
        return d3d12_heap;
    }

    ID3D12Device4* m_d3d12_device;

    std::map<Key, std::list<D3DDescriptorHeap>> m_descriptor_heaps;
};

class D3DDescriptorAllocatorWrapper {
public:

    static void initialize(ID3D12Device4* d3d12_device) {

    }

private:


};

}