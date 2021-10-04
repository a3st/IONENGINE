// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DSampler : public Sampler {
public:

    D3DSampler(const SamplerDesc& sampler_desc) : m_desc(sampler_desc) {

        D3D12_SAMPLER_DESC d3d12_sampler_desc{};
        d3d12_sampler_desc.Filter = gfx_to_d3d12_filter(sampler_desc.filter);
        d3d12_sampler_desc.AddressU = gfx_to_d3d12_texture_address_mode(sampler_desc.mode);
        d3d12_sampler_desc.AddressV = gfx_to_d3d12_texture_address_mode(sampler_desc.mode);
        d3d12_sampler_desc.AddressW = gfx_to_d3d12_texture_address_mode(sampler_desc.mode);
        d3d12_sampler_desc.ComparisonFunc = gfx_to_d3d12_comparison_func(sampler_desc.func);
        d3d12_sampler_desc.MaxLOD = std::numeric_limits<float>::max();
        d3d12_sampler_desc.MaxAnisotropy = 1;

        m_d3d12_desc = d3d12_sampler_desc;
    }

    const SamplerDesc& get_desc() const override { return m_desc; }

    const D3D12_SAMPLER_DESC& get_d3d12_desc() const { return m_d3d12_desc; }

private:

    D3D12_SAMPLER_DESC m_d3d12_desc;
    
    SamplerDesc m_desc;
};

}