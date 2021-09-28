// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DDescriptorSetLayout : public DescriptorSetLayout  {
public:

    D3DDescriptorSetLayout(ID3D12Device4* d3d12_device, const std::vector<DescriptorSetLayoutBinding>& bindings) {

    }

    ID3D12RootSignature* get_d3d12_root_signature() { return m_d3d12_root_signature.get(); }

private:

    winrt::com_ptr<ID3D12RootSignature> m_d3d12_root_signature;

    
};

}