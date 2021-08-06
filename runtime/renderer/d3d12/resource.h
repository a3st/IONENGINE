// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DResource : public Resource {
public:

    D3DResource(winrt::com_ptr<ID3D12Device4>& device, const D3D12_RESOURCE_DESC& desc) : m_device(device), m_resource_desc(desc) {

    }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;

    winrt::com_ptr<ID3D12Resource> m_d3d12_resource;
    D3D12_RESOURCE_DESC m_resource_desc;
    
};

}