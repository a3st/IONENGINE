// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Adapter final {
public:

    Adapter(const Instance& instance) : m_instance(instance) {
        
        ComPtr<IDXGIAdapter1> adapter;
        for(uint32 i = 0; ; m_instance.m_factory->EnumAdapters1(i, adapter.GetAddressOf()), ++i) {
            
            DXGI_ADAPTER_DESC1 adapter_desc = {};
            throw_if_failed(adapter->GetDesc1(&adapter_desc));
            if(::D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), nullptr)) {
                
            }    
        }
    }
    

private:

    const Instance& m_instance;
    ComPtr<IDXGIAdapter4> m_adapter;
    
};

}