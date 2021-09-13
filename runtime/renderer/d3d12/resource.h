// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DResource : public Resource {
public:

    D3DResource(winrt::com_ptr<ID3D12Device4>& device, const D3D12_RESOURCE_DESC& desc) : m_device(device), m_desc(desc) {

        m_format = static_cast<Format>(desc.Format);
    }

    D3DResource(winrt::com_ptr<ID3D12Device4>& device, const winrt::com_ptr<ID3D12Resource>& resource) : m_device(device), m_d3d12_resource(resource) {

        m_desc = m_d3d12_resource->GetDesc();
        m_format = static_cast<Format>(m_d3d12_resource->GetDesc().Format);
        m_width = m_d3d12_resource->GetDesc().Width;
        m_height = m_d3d12_resource->GetDesc().Height;
    }

    void bind_memory(Memory& memory, const uint64 offset) override {
        
        m_memory = static_cast<D3DMemory&>(memory);

        ResourceState resource_state = ResourceState::Common;
        if(m_memory.value().get().get_type() == MemoryType::Upload) {
            resource_state = ResourceState::GenericRead;
        }

        ASSERT_SUCCEEDED(m_device.get()->CreatePlacedResource(
            m_memory.value().get().get_heap().get(),
            offset,
            &std::get<D3D12_RESOURCE_DESC>(m_desc),
            convert_resource_state(resource_state),
            nullptr,
            __uuidof(ID3D12Resource), m_d3d12_resource.put_void()
        ));
    }

    Format get_format() const override { return m_format; }
    uint64 get_width() const override { return m_width; }
    uint32 get_height() const override { return m_height; }

    std::variant<
        D3D12_RESOURCE_DESC,
        D3D12_SAMPLER_DESC
    > get_desc() { return m_desc; }

    winrt::com_ptr<ID3D12Resource>& get_resource() { return m_d3d12_resource; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12Resource> m_d3d12_resource;

    std::variant<
        D3D12_RESOURCE_DESC,
        D3D12_SAMPLER_DESC
    > m_desc;

    Format m_format;
    uint32 m_width;
    uint32 m_height;

    std::optional<std::reference_wrapper<D3DMemory>> m_memory;
};

}