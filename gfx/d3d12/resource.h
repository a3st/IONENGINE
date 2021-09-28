// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DResource : public Resource {
public:

    D3DResource(ID3D12Device4* d3d12_device, const ResourceType resource_type, const ResourceDesc& resource_desc) : m_d3d12_device(d3d12_device), m_desc(resource_desc) {

        switch(resource_type) {
            case ResourceType::Buffer:
            case ResourceType::Texture: {
                D3D12_RESOURCE_DESC d3d12_resource_desc{};
                d3d12_resource_desc.Dimension = d3d12_resource_dimension_to_gfx_enum(resource_desc.dimension);
                d3d12_resource_desc.Alignment = 0;
                        
                if(resource_desc.flags & ResourceFlags::ConstantBuffer) {
                    d3d12_resource_desc.Width = (resource_desc.width + 255) & ~255;
                } else {
                    d3d12_resource_desc.Width = resource_desc.width;
                }

                d3d12_resource_desc.Height = resource_desc.height;
                d3d12_resource_desc.DepthOrArraySize = resource_desc.array_size;
                d3d12_resource_desc.MipLevels = resource_desc.mip_levels;
                d3d12_resource_desc.Format = static_cast<DXGI_FORMAT>(resource_desc.format);
                d3d12_resource_desc.SampleDesc.Count = resource_desc.sample_count;
                d3d12_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
                        
                if(resource_desc.flags & ResourceFlags::RenderTarget) {
                    d3d12_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
                } else if(resource_desc.flags & ResourceFlags::DepthStencil) {
                    d3d12_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
                } else if(resource_desc.flags & ResourceFlags::UnorderedAccess) {
                    d3d12_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
                }
                m_d3d12_desc = d3d12_resource_desc;
                break;
            }
            default: assert(false && "resource type should be Buffer or Texture when passed ResourceDesc"); break;
        }
    }

    D3DResource(ID3D12Device4* d3d12_device, const ResourceType resource_type, const winrt::com_ptr<ID3D12Resource>& resource, const ResourceFlags resource_flags) 
        : m_d3d12_device(d3d12_device), m_type(resource_type), m_d3d12_resource(resource) {

        m_d3d12_desc = m_d3d12_resource->GetDesc();

        ResourceDesc resource_desc{};
        switch(m_d3d12_desc.Dimension) {
            case D3D12_RESOURCE_DIMENSION_BUFFER: resource_desc.dimension = ViewDimension::Buffer; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: resource_desc.dimension = ViewDimension::Texture1D; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: resource_desc.dimension = ViewDimension::Texture2D; break;
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: resource_desc.dimension = ViewDimension::Texture3D; break;
            case D3D12_RESOURCE_DIMENSION_UNKNOWN: resource_desc.dimension = ViewDimension::Unknown; break;
        }
        resource_desc.format = static_cast<Format>(m_d3d12_desc.Format);
        resource_desc.width = m_d3d12_desc.Width;
        resource_desc.height = m_d3d12_desc.Height;
        resource_desc.mip_levels = m_d3d12_desc.MipLevels;
        resource_desc.array_size = m_d3d12_desc.DepthOrArraySize;
        resource_desc.sample_count = m_d3d12_desc.SampleDesc.Count;
        resource_desc.flags = resource_flags;

        m_desc = resource_desc;
    }

    void bind_memory(Memory* memory, const uint64 offset) override {
        
        assert((memory->get_flags() & m_desc.flags) && "flags binded error");

        ResourceState resource_state = ResourceState::Common;
        if(memory->get_type() == MemoryType::Upload) {
            resource_state = ResourceState::GenericRead;
        }

        m_memory = static_cast<D3DMemory*>(memory);

        THROW_IF_FAILED(
            m_d3d12_device->CreatePlacedResource(
                m_memory->get_d3d12_heap(),
                offset,
                &m_d3d12_desc,
                d3d12_resource_state_to_gfx_enum(resource_state),
                nullptr,
                __uuidof(ID3D12Resource), m_d3d12_resource.put_void()
            )
        );
    }

    byte* map() override {
        D3D12_RANGE range{};
        byte* dst_data;
        THROW_IF_FAILED(m_d3d12_resource->Map(0, &range, reinterpret_cast<void**>(&dst_data)));
        return dst_data;
    }

    void unmap() override {
        D3D12_RANGE range{};
        m_d3d12_resource->Unmap(0, &range);
    }

    ResourceType get_type() const override { return m_type; }
    const ResourceDesc& get_desc() const override { return m_desc; }

    const D3D12_RESOURCE_DESC& get_d3d12_desc() const { return m_d3d12_desc; }
    ID3D12Resource* get_d3d12_resource() { return m_d3d12_resource.get(); }

private:

    ID3D12Device4* m_d3d12_device;

    winrt::com_ptr<ID3D12Resource> m_d3d12_resource;
    D3D12_RESOURCE_DESC m_d3d12_desc;

    D3DMemory* m_memory;

    ResourceType m_type;
    ResourceDesc m_desc;
};

}