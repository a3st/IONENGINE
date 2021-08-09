// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DResource : public Resource {
public:

    D3DResource(winrt::com_ptr<ID3D12Device4>& device, const D3D12_RESOURCE_DESC& desc) : m_device(device), m_resource_desc(desc) {

    }

    void bind_memory(const std::shared_ptr<Memory>& memory, const uint64 offset) override {
        
        m_memory = std::static_pointer_cast<D3DMemory>(memory);

        ResourceState resource_state = ResourceState::Common;
        if(m_memory->get_type() == MemoryType::Upload) {
            resource_state = ResourceState::GenericRead;
        }

        m_device.get()->CreatePlacedResource(
            m_memory->get_heap().get(),
            offset,
            &std::get<D3D12_RESOURCE_DESC>(m_resource_desc),
            convert_enum(resource_state),
            nullptr,
            __uuidof(ID3D12Resource), m_d3d12_resource.put_void()
        );
    }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12Resource> m_d3d12_resource;

    std::variant<
        D3D12_RESOURCE_DESC,
        D3D12_SAMPLER_DESC
    > m_resource_desc;

    std::shared_ptr<D3DMemory> m_memory;
};

}