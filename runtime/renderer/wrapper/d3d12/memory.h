// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DMemory : public Memory {
public:

    D3DMemory(ID3D12Device4* d3d12_device, const MemoryType memory_type, const usize size, const uint32 alignment, const BufferFlags buffer_flags) 
        : m_type(memory_type) {

        D3D12_HEAP_DESC heap_desc{};
        heap_desc.Properties.Type = conv_heap_type(memory_type);
        heap_desc.SizeInBytes = size;
        heap_desc.Alignment = alignment;
        switch(buffer_flags) {
            case BufferFlags::ConstantBuffer:
            case BufferFlags::IndexBuffer:
            case BufferFlags::VertexBuffer:
            case BufferFlags::UnorderedAccess: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS; break;
            case BufferFlags::ShaderResource: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES; break;
            case BufferFlags::DepthStencil:
            case BufferFlags::RenderTarget: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES; break;
        }
        ASSERT_SUCCEEDED(d3d12_device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), m_d3d12_heap.put_void()));
    }

    MemoryType get_type() const override { return m_type; }
    BufferFlags get_flags() const override { return m_flags; }

    ID3D12Heap* get_d3d12_heap() { return m_d3d12_heap.get(); }
    
private:

    winrt::com_ptr<ID3D12Heap> m_d3d12_heap;
    MemoryType m_type;
    BufferFlags m_flags;
};

}