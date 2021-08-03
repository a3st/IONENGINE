// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DAdapter : public Adapter {
public:

    D3DAdapter(const ComPtr<IDXGIFactory4>& instance, const ComPtr<IDXGIAdapter1>& adapter) : m_dxgi_factory(instance), m_dxgi_adapter(adapter) {

        DXGI_ADAPTER_DESC adapter_desc{};
        adapter->GetDesc(&adapter_desc);
        
        m_name = wsts(adapter_desc.Description);
        m_memory = adapter_desc.DedicatedVideoMemory;
        m_vendor_id = adapter_desc.VendorId;
        m_device_id = adapter_desc.DeviceId;
    }

    std::unique_ptr<Device> create_device() {
        return std::make_unique<D3DDevice>(m_dxgi_factory, m_dxgi_adapter);
    }

    const std::string& get_name() const override { return m_name; }
    usize get_memory() const override { return m_memory; }
    uint32 get_device_id() const override { return m_device_id; }
    uint32 get_vendor_id() const override { return m_vendor_id; }

    const ComPtr<IDXGIAdapter1>& get_adapter() const { return m_dxgi_adapter; }
    const ComPtr<IDXGIFactory4>& get_factory() const { return m_dxgi_factory; }
    
private:

    const ComPtr<IDXGIFactory4>& m_dxgi_factory;

    ComPtr<IDXGIAdapter1> m_dxgi_adapter;

    std::string m_name;
    usize m_memory;
    uint32 m_vendor_id;
    uint32 m_device_id;
};

}