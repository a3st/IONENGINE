// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Instance final {
friend class Adapter;
friend class Device;
public:

    Instance() {
        
        throw_if_failed(::CreateDXGIFactory2(0, IID_PPV_ARGS(&m_factory)));
        
        throw_if_failed(::D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug)));
        m_debug->EnableDebugLayer();
    }

private:

    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<ID3D12Debug> m_debug;
};

}