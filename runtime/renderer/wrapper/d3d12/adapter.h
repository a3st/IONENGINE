// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class D3DAdapter : public Adapter {
public:

    D3DAdapter(IDXGIFactory4* dxgi_factory, IDXGIAdapter1* dxgi_adapter) 
        : m_dxgi_factory(dxgi_factory), m_dxgi_adapter(dxgi_adapter) {
    
        DXGI_ADAPTER_DESC adapter_desc{};
        adapter->GetDesc(&adapter_desc);
        
        m_name = wsts(adapter_desc.Description);
        m_memory = adapter_desc.DedicatedVideoMemory;
        m_vendor_id = adapter_desc.VendorId;
        m_device_id = adapter_desc.DeviceId;
    }

    const std::string& get_name() const override { return m_name; }
    usize get_memory() const override { return m_memory; }
    uint32 get_device_id() const override { return m_device_id; }
    uint32 get_vendor_id() const override { return m_vendor_id; }

    IDXGIAdapter1* get_dxgi_adapter() { return m_dxgi_adapter; }
    IDXGIFactory4* get_dxgi_factory() { return m_dxgi_factory; }

    std::unique_ptr<Device> create_device() override {
        return std::make_unique<D3DDevice>(m_dxgi_factory, m_dxgi_adapter);
    }
    
private:

    const IDXGIFactory4* m_dxgi_factory;
    const IDXGIAdapter1* m_dxgi_adapter;

    std::string m_name;
    usize m_memory;
    uint32 m_vendor_id;
    uint32 m_device_id;
};

}