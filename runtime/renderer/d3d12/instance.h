// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DInstance : public Instance {
public:

    D3DInstance() {

        uint32 flags = 0;
#ifdef NDEBUG
        if(SUCCEEDED(D3D12GetDebugInterface( __uuidof(ID3D12Debug), m_d3d12_debug.put_void()))) {
            m_d3d12_debug->EnableDebugLayer();
        }
        flags = DXGI_CREATE_FACTORY_DEBUG;
        ASSERT_SUCCEEDED(DXGIGetDebugInterface1(0,  __uuidof(IDXGIDebug), m_dxgi_debug.put_void()));
#endif

        ASSERT_SUCCEEDED(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory4), m_dxgi_factory.put_void()));
    }

    ~D3DInstance() {
#ifdef NDEBUG
        ASSERT_SUCCEEDED(m_dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL));
#endif
    }

    std::vector<std::unique_ptr<Adapter>> enumerate_adapters() override {
        std::vector<std::unique_ptr<Adapter>> out_adapters;

        for(uint32 i = 0; ; ++i) {

            winrt::com_ptr<IDXGIAdapter1> temp_adapter;
            if(DXGI_ERROR_NOT_FOUND == m_dxgi_factory->EnumAdapters1(i, temp_adapter.put())) {
                break;
            }

            if(SUCCEEDED(D3D12CreateDevice(temp_adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) {
                out_adapters.emplace_back(std::make_unique<D3DAdapter>(m_dxgi_factory, temp_adapter));
            }
        }
        return out_adapters;
    }

    const winrt::com_ptr<IDXGIFactory4>& get_factory() const { return m_dxgi_factory; }
    const winrt::com_ptr<ID3D12Debug>& get_debug() const { return m_d3d12_debug; }

private:

    winrt::com_ptr<IDXGIFactory4> m_dxgi_factory;
    winrt::com_ptr<ID3D12Debug> m_d3d12_debug;
    winrt::com_ptr<IDXGIDebug> m_dxgi_debug;
};

std::unique_ptr<Instance> create_unique_instance() {
    return std::make_unique<D3DInstance>();
}

}