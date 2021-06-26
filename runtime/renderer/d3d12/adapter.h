// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/fmt.h"

namespace ionengine::renderer {

class Adapter final {
public:

    Adapter(const Instance& instance) : 
        m_instance(instance), 
        m_id(0), m_name("Unknown device"), m_memory(0) {
        
        ComPtr<IDXGIAdapter1> temp_adapter;
        for(uint32 i = 0; m_instance.get_factory_ptr()->EnumAdapters1(i, temp_adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i) {

            DXGI_ADAPTER_DESC1 adapter_desc = {};
            throw_if_failed(temp_adapter->GetDesc1(&adapter_desc));

            if(adapter_desc.Flags != DXGI_ADAPTER_FLAG_SOFTWARE) {
                if(SUCCEEDED(D3D12CreateDevice(temp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), nullptr))) {
                    m_memory = adapter_desc.DedicatedVideoMemory;
                    m_id = adapter_desc.DeviceId;
                    m_name = wsts(adapter_desc.Description);
                    temp_adapter.As(&m_adapter_ptr);
                    break;
                }
            }
        }
    }

    const std::string& get_name() const { return m_name; }
    uint64 get_id() const { return m_id; }
    usize get_memory() const { return m_memory; }
    const ComPtr<IDXGIAdapter4>& get_adapter_ptr() const { return m_adapter_ptr; }

private:

    const Instance& m_instance;

    ComPtr<IDXGIAdapter4> m_adapter_ptr;
    uint64 m_id;
    std::string m_name;
    usize m_memory;
};

}