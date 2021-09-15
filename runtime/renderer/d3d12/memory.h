// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DMemory : public Memory {
public:

    D3DMemory(winrt::com_ptr<ID3D12Device4>& device, const MemoryType memory_type, usize size, uint32 alignment, const ResourceFlags flags) : m_device(device) {

        D3D12_HEAP_DESC heap_desc{};
        heap_desc.Properties.Type = convert_heap_type(memory_type);
        heap_desc.SizeInBytes = size;
        heap_desc.Alignment = alignment;
        switch(memory_type) {
            case ResourceFlags::ConstantBuffer:
            case ResourceFlags::IndexBuffer:
            case ResourceFlags::VertexBuffer:
            case ResourceFlags::UnorderedAccess: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS; break;
            case ResourceFlags::ShaderResource: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES; break;
            case ResourceFlags::DepthStencil:
            case ResourceFlags::RenderTarget: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES; break;
        }

        ASSERT_SUCCEEDED(m_device.get()->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), m_d3d12_heap.put_void()));
    }

    MemoryType get_type() const override { return m_memory_type; }

    winrt::com_ptr<ID3D12Heap>& get_d3d12_heap() { return m_d3d12_heap; }
    
private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12Heap> m_d3d12_heap;
    MemoryType m_memory_type;
};

}