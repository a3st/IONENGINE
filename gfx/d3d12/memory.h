// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DMemory : public Memory {
public:

    D3DMemory(ID3D12Device4* d3d12_device, const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) 
        : m_type(memory_type) {

        D3D12_HEAP_DESC heap_desc{};
        heap_desc.Properties.Type = d3d12_heap_type_to_gfx_enum(memory_type);
        heap_desc.SizeInBytes = size;
        heap_desc.Alignment = alignment;
        switch(resource_flags) {
            case ResourceFlags::ConstantBuffer:
            case ResourceFlags::IndexBuffer:
            case ResourceFlags::VertexBuffer:
            case ResourceFlags::UnorderedAccess: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS; break;
            case ResourceFlags::ShaderResource: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES; break;
            case ResourceFlags::DepthStencil:
            case ResourceFlags::RenderTarget: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES; break;
        }
        THROW_IF_FAILED(d3d12_device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), m_d3d12_heap.put_void()));
    }

    MemoryType get_type() const override { return m_type; }
    ResourceFlags get_flags() const override { return m_flags; }

    ID3D12Heap* get_d3d12_heap() { return m_d3d12_heap.get(); }
    
private:

    winrt::com_ptr<ID3D12Heap> m_d3d12_heap;
    MemoryType m_type;
    ResourceFlags m_flags;
};

}