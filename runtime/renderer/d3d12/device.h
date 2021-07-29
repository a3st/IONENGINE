// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

struct AdapterConfig {

    uint64 device_id;
    uint64 vendor_id;
    std::string device_name;
    usize dedicated_memory;
};

class Device {
friend class Swapchain;
friend class ImageView;
friend class Image;
friend class FramebufferAttachment;
public:

    Device() {
        
        initialize_instance();
        initialize_adapter();
        initialize_device();
    }

    Device(const Device&) = delete;
    Device(Device&&) noexcept = delete;

    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) noexcept = delete;

    const AdapterConfig get_adapter_config() const { return m_adapter_config; }

private:

    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<ID3D12Debug> m_debug;
    ComPtr<IDXGIAdapter4> m_adapter;
    ComPtr<ID3D12Device4> m_device;

    AdapterConfig m_adapter_config;

    struct {
        ComPtr<ID3D12CommandQueue> direct;
        ComPtr<ID3D12CommandQueue> compute;
        ComPtr<ID3D12CommandQueue> transfer;
    } m_queues;

    void initialize_instance() {

        uint32 factory_flags = 0;
        factory_flags = DXGI_CREATE_FACTORY_DEBUG;

        throw_if_failed(CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&m_factory)));
        throw_if_failed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug)));
        m_debug->EnableDebugLayer();
    }

    void initialize_adapter() {

        ComPtr<IDXGIAdapter1> temp_adapter;
        for(uint32 i = 0; m_factory->EnumAdapters1(i, temp_adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; ++i) {

            DXGI_ADAPTER_DESC1 adapter_desc{};
            throw_if_failed(temp_adapter->GetDesc1(&adapter_desc));

            if(adapter_desc.Flags != DXGI_ADAPTER_FLAG_SOFTWARE) {

                if(SUCCEEDED(D3D12CreateDevice(temp_adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), nullptr))) {
                    m_adapter_config.vendor_id = adapter_desc.VendorId;
                    m_adapter_config.device_id = adapter_desc.DeviceId;
                    m_adapter_config.dedicated_memory = adapter_desc.DedicatedVideoMemory;
                    m_adapter_config.device_name = wsts(adapter_desc.Description);
                    temp_adapter.As(&m_adapter);
                    break;
                }
            }
        }
    }

    void initialize_device() {

        throw_if_failed(D3D12CreateDevice(m_adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        throw_if_failed(m_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queues.direct)));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        throw_if_failed(m_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queues.transfer)));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        throw_if_failed(m_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&m_queues.compute)));
    }

};

}