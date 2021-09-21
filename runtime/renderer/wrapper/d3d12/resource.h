// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DResource : public Resource {
public:

    D3DResource(ID3D12Device4* device, const ResourceType type, const std::variant<ResourceDesc, SamplerDesc>& desc) : m_device(device), m_desc(desc) {

        std::visit(
            [&](const ResourceDesc& resource_desc) {

                switch(type) {
                    case ResourceType::Buffer:
                    case ResourceType::Texture: {
                        D3D12_RESOURCE_DESC d3d12_resource_desc{};
                        d3d12_resource_desc.Dimension = convert_resource_dimension(resource_desc.dimension);
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
                        m_d3d12_resource_desc = d3d12_resource_desc;
                        break;
                    }
                    default: assert(false && "resource type should be Buffer or Texture when passed ResourceDesc"); break;
                }
            },
            [&](const SamplerDesc& sampler_desc) {

                if(type == ResourceType::Sampler) {
                    D3D12_SAMPLER_DESC d3d12_sampler_desc{};
                    d3d12_sampler_desc.Filter = convert_filter(sampler_desc.filter);
                    d3d12_sampler_desc.AddressU = convert_texture_address_mode(sampler_desc.mode);
                    d3d12_sampler_desc.AddressV = convert_texture_address_mode(sampler_desc.mode);
                    d3d12_sampler_desc.AddressW = convert_texture_address_mode(sampler_desc.mode);
                    d3d12_sampler_desc.ComparisonFunc = convert_comparison_func(sampler_desc.func);
                    d3d12_sampler_desc.MaxLOD = std::numeric_limits<float>::max();
                    d3d12_sampler_desc.MaxAnisotropy = 1;
                    m_d3d12_resource_desc = d3d12_sampler_desc;
                } else {
                    assert(false && "resource type should be Sampler when passed SamplerDesc");
                }
            },
            desc
        );
    }

    D3DResource(ID3D12Device4* device, const ResourceType type, const winrt::com_ptr<ID3D12Resource>& resource, const ResourceFlags flags) 
        : m_device(device), m_resource_type(type), m_d3d12_resource(resource) {

        m_d3d12_resource_desc = m_d3d12_resource->GetDesc();

        ResourceDesc resource_desc{};
        switch(m_d3d12_resource->GetDesc().Dimension) {
            case D3D12_RESOURCE_DIMENSION_BUFFER: {
                resource_desc.dimension = ViewDimension::Buffer; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE1D: {
                resource_desc.dimension = ViewDimension::Texture1D; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE2D: {
                resource_desc.dimension = ViewDimension::Texture2D; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_TEXTURE3D: {
                resource_desc.dimension = ViewDimension::Texture3D; 
                break;
            }
            case D3D12_RESOURCE_DIMENSION_UNKNOWN: {
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
        resource_desc.flags = flags;

        m_resource_desc = resource_desc;
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

    ResourceType get_type() const { return m_type; }

    const std::variant<
        ResourceDesc,
        SamplerDesc
    >& get_desc() const override { return m_desc; }

    const std::variant<
        D3D12_RESOURCE_DESC,
        D3D12_SAMPLER_DESC
    >& get_d3d12_desc() const { return m_d3d12_desc; }

    winrt::com_ptr<ID3D12Resource>& get_d3d12_resource() { return m_d3d12_resource; }

private:

    ID3D12Device4& m_device;

    winrt::com_ptr<ID3D12Resource> m_d3d12_resource;

    std::variant<
        D3D12_RESOURCE_DESC,
        D3D12_SAMPLER_DESC
    > m_d3d12_desc;

    D3DMemory* m_memory;

    std::variant<
        ResourceDesc,
        SamplerDesc
    > m_desc;

    ResourceType m_type;
};

}