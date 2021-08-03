// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DInstance : public Instance {
public:

    D3DInstance() {

        uint32 flags = 0;
#ifdef NDEBUG
        ComPtr<ID3D12Debug> debug_controller;
        if(ASSERT_SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)))) {
            debug_controller->EnableDebugLayer();
        }
        flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        ASSERT_SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_dxgi_factory)));
    }

    std::vector<std::unique_ptr<Adapter>> enumerate_adapters() override {
        std::vector<std::unique_ptr<Adapter>> out_adapters;

        ComPtr<IDXGIAdapter1> temp_adapter;
        for(uint32 i = 0; m_dxgi_factory->EnumAdapters1(i, temp_adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i) {
            if(SUCCEEDED(D3D12CreateDevice(temp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), nullptr))) {
                out_adapters.emplace_back(std::make_unique<D3DAdapter>(m_dxgi_factory, temp_adapter));
            }
        }
        return out_adapters;
    }

    const ComPtr<IDXGIFactory4>& get_factory() const { return m_dxgi_factory; }

private:

    ComPtr<IDXGIFactory4> m_dxgi_factory;
};

std::unique_ptr<Instance> create_unique_instance() {
    return std::make_unique<D3DInstance>();
}

}