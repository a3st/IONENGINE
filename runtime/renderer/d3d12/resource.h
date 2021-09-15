// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DResource : public Resource {
public:

    D3DResource(winrt::com_ptr<ID3D12Device4>& device, const ResourceDesc& resource_desc, const ResourceFlags flags) : m_device(device), m_resource_desc(resource_desc) {

        /*usize align_size = buffer_size;
        if(flags & ResourceFlags::ConstantBuffer) {
            align_size = (buffer_size + 255) & ~255;
        }

        D3D12_RESOURCE_DESC resource_desc{};
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resource_desc.Alignment = 0;
        resource_desc.Width = align_size;
        resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 1;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        if(flags & ResourceFlags::RenderTarget) {
            resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        } else if(flags & ResourceFlags::DepthStencil) {
            resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        } else if(flags & ResourceFlags::UnorderedAccess) {
            resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }*/
    }

    D3DResource(winrt::com_ptr<ID3D12Device4>& device, const SamplerDesc& sampler_desc) : m_device(device), m_resource_desc(sampler_desc) {

        D3D12_SAMPLER_DESC d3d12_sampler_desc{};
        d3d12_sampler_desc.Filter = convert_filter(sampler_desc.filter);
        d3d12_sampler_desc.AddressU = convert_texture_address_mode(sampler_desc.mode);
        d3d12_sampler_desc.AddressV = convert_texture_address_mode(sampler_desc.mode);
        d3d12_sampler_desc.AddressW = convert_texture_address_mode(sampler_desc.mode);
        d3d12_sampler_desc.ComparisonFunc = convert_comparison_func(sampler_desc.func);
        d3d12_sampler_desc.MaxLOD = std::numeric_limits<float>::max();
        d3d12_sampler_desc.MaxAnisotropy = 1;

        m_d3d12_resource_desc = d3d12_sampler_desc;
    }

    D3DResource(winrt::com_ptr<ID3D12Device4>& device, const winrt::com_ptr<ID3D12Resource>& resource) : m_device(device), m_d3d12_resource(resource) {

        m_d3d12_resource_desc = m_d3d12_resource->GetDesc();

        ResourceDesc resource_desc{};
        switch(m_d3d12_resource->GetDesc().Dimension) {
            case D3D12_RESOURCE_DIMENSION_BUFFER: {
                resource_desc.resource_type = ResourceType::Buffer;
                resource_desc.dimension = ViewDimension::Buffer; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: {
                resource_desc.resource_type = ResourceType::Texture;
                resource_desc.dimension = ViewDimension::Texture1D; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: {
                resource_desc.resource_type = ResourceType::Texture;
                resource_desc.dimension = ViewDimension::Texture2D; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: {
                resource_desc.resource_type = ResourceType::Texture;
                resource_desc.dimension = ViewDimension::Texture3D; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_UNKNOWN: {
                resource_desc.resource_type = ResourceType::Unknown;
                resource_desc.dimension = ViewDimension::Unknown; 
                break;
            }
        }
        resource_desc.format = static_cast<Format>(m_d3d12_resource->GetDesc().Format);
        resource_desc.width = m_d3d12_resource->GetDesc().Width;
        resource_desc.height = m_d3d12_resource->GetDesc().Height;
        resource_desc.mip_levels = m_d3d12_resource->GetDesc().MipLevels;
        resource_desc.array_size = m_d3d12_resource->GetDesc().DepthOrArraySize;
        resource_desc.sample_count = m_d3d12_resource->GetDesc().SampleDesc.Count;
    }

    void bind_memory(Memory& memory, const uint64 offset) override {
        
        m_memory = static_cast<D3DMemory&>(memory);

        ResourceState resource_state = ResourceState::Common;
        if(m_memory.value().get().get_type() == MemoryType::Upload) {
            resource_state = ResourceState::GenericRead;
        }

        ASSERT_SUCCEEDED(m_device.get()->CreatePlacedResource(
            m_memory.value().get().get_d3d12_heap().get(),
            offset,
            &std::get<D3D12_RESOURCE_DESC>(m_d3d12_resource_desc),
            convert_resource_state(resource_state),
            nullptr,
            __uuidof(ID3D12Resource), m_d3d12_resource.put_void()
        ));
    }

    const std::variant<
        ResourceDesc,
        SamplerDesc
    >& get_desc() const override { return m_resource_desc; }

    const std::variant<
        D3D12_RESOURCE_DESC,
        D3D12_SAMPLER_DESC
    >& get_d3d12_desc() const { return m_d3d12_resource_desc; }

    winrt::com_ptr<ID3D12Resource>& get_d3d12_resource() { return m_d3d12_resource; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12Resource> m_d3d12_resource;

    std::variant<
        D3D12_RESOURCE_DESC,
        D3D12_SAMPLER_DESC
    > m_d3d12_resource_desc;

    std::optional<std::reference_wrapper<D3DMemory>> m_memory;

    std::variant<
        ResourceDesc,
        SamplerDesc
    > m_resource_desc;
};

}