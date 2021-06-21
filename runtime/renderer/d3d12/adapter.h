// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Adapter final {
friend class Device;
public:

    Adapter(const Instance& instance) : m_instance(instance) {
        
        ComPtr<IDXGIAdapter1> adapter;
        for(uint32 i = 0; m_instance.m_factory->EnumAdapters1(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i) {

            DXGI_ADAPTER_DESC1 adapter_desc = {};
            throw_if_failed(adapter->GetDesc1(&adapter_desc));

            if(adapter_desc.Flags != DXGI_ADAPTER_FLAG_SOFTWARE) {
                if(SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), nullptr))) {
                    
                    m_device_memory = adapter_desc.DedicatedVideoMemory;

                    m_device_id = adapter_desc.DeviceId;
                    m_device_name = wsts(adapter_desc.Description);

                    adapter.As(&m_adapter);
                    break;
                }
            }
        }
    }

    const std::string& get_device_name() const {
        return m_device_name;
    }

    uint64 get_device_id() const { 
        return m_device_id; 
    }

    usize get_device_memory() const {
        return m_device_memory;
    }

private:

    const Instance& m_instance;
    ComPtr<IDXGIAdapter4> m_adapter;

    uint64 m_device_id;
    std::string m_device_name;
    usize m_device_memory;
};

}