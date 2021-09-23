// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DBuffer : public Buffer {
public:

    D3DBuffer(ID3D12Device4* d3d12_device, const BufferType buffer_type, const BufferDesc& buffer_desc) : m_d3d12_device(d3d12_device), m_desc(buffer_desc) {

        switch(buffer_type) {
            case BufferType::Buffer:
            case BufferType::Texture: {
                D3D12_RESOURCE_DESC resource_desc{};
                resource_desc.Dimension = conv_resource_dimension(buffer_desc.dimension);
                resource_desc.Alignment = 0;
                        
                if(buffer_desc.flags & BufferFlags::ConstantBuffer) {
                    resource_desc.Width = (buffer_desc.width + 255) & ~255;
                } else {
                    resource_desc.Width = buffer_desc.width;
                }

                resource_desc.Height = buffer_desc.height;
                resource_desc.DepthOrArraySize = buffer_desc.array_size;
                resource_desc.MipLevels = buffer_desc.mip_levels;
                resource_desc.Format = static_cast<DXGI_FORMAT>(buffer_desc.format);
                resource_desc.SampleDesc.Count = buffer_desc.sample_count;
                resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                        
                if(buffer_desc.flags & BufferFlags::RenderTarget) {
                    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                } else if(buffer_desc.flags & BufferFlags::DepthStencil) {
                    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
                } else if(buffer_desc.flags & BufferFlags::UnorderedAccess) {
                    resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
                }
                m_d3d12_desc = resource_desc;
                break;
            }
            default: assert(false && "resource type should be Buffer or Texture when passed ResourceDesc"); break;
        }
    }

    D3DBuffer(ID3D12Device4* d3d12_device, const BufferType buffer_type, const winrt::com_ptr<ID3D12Resource>& resource, const BufferFlags buffer_flags) 
        : m_d3d12_device(d3d12_device), m_type(buffer_type), m_d3d12_resource(resource) {

        m_d3d12_desc = m_d3d12_resource->GetDesc();

        BufferDesc buffer_desc{};
        switch(m_d3d12_desc.Dimension) {
            case D3D12_RESOURCE_DIMENSION_BUFFER: buffer_desc.dimension = ViewDimension::Buffer; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: buffer_desc.dimension = ViewDimension::Texture1D; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: buffer_desc.dimension = ViewDimension::Texture2D; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: buffer_desc.dimension = ViewDimension::Texture3D; break;
            case D3D12_RESOURCE_DIMENSION_UNKNOWN: buffer_desc.dimension = ViewDimension::Unknown; break;
        }
        buffer_desc.format = static_cast<Format>(m_d3d12_desc.Format);
        buffer_desc.width = m_d3d12_desc.Width;
        buffer_desc.height = m_d3d12_desc.Height;
        buffer_desc.mip_levels = m_d3d12_desc.MipLevels;
        buffer_desc.array_size = m_d3d12_desc.DepthOrArraySize;
        buffer_desc.sample_count = m_d3d12_desc.SampleDesc.Count;
        buffer_desc.flags = buffer_flags;

        m_desc = buffer_desc;
    }

    void bind_memory(Memory* memory, const uint64 offset) override {
        
        assert((memory->get_flags() & m_desc.flags) && "flags binded error");

        BufferState buffer_state = BufferState::Common;
        if(memory->get_type() == MemoryType::Upload) {
            buffer_state = BufferState::GenericRead;
        }

        m_memory = static_cast<D3DMemory*>(memory);

        THROW_IF_FAILED(
            m_d3d12_device->CreatePlacedResource(
                m_memory->get_d3d12_heap(),
                offset,
                &m_d3d12_desc,
                conv_resource_state(buffer_state),
                nullptr,
                __uuidof(ID3D12Resource), m_d3d12_resource.put_void()
            )
        );
    }

    byte* map() override {
        D3D12_RANGE range{};
        byte* dst_data;
        ASSERT_SUCCEEDED(m_d3d12_resource->Map(0, &range, reinterpret_cast<void**>(&dst_data)));
        return dst_data;
    }

    void unmap() override {
        D3D12_RANGE range{};
        m_d3d12_resource->Unmap(0, &range);
    }

    BufferType get_type() const override { return m_type; }
    const BufferDesc& get_desc() const override { return m_desc; }

    const D3D12_RESOURCE_DESC& get_d3d12_desc() const { return m_d3d12_desc; }
    ID3D12Resource* get_d3d12_resource() { return m_d3d12_resource.get(); }

private:

    ID3D12Device4* m_d3d12_device;

    winrt::com_ptr<ID3D12Resource> m_d3d12_resource;
    D3D12_RESOURCE_DESC m_d3d12_desc;

    D3DMemory* m_memory;

    BufferType m_type;
    BufferDesc m_desc;
};

}