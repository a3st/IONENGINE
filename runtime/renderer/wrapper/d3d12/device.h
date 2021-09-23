// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DDevice : public Device {
public:

    D3DDevice(const uint32 adapter_index) {

        uint32 flags = 0;
#ifdef NDEBUG
        if(SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), m_d3d12_debug.put_void()))) {
            m_d3d12_debug->EnableDebugLayer();
        }
        flags = DXGI_CREATE_FACTORY_DEBUG;
#endif
        ASSERT_SUCCEEDED(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory4), m_dxgi_factory.put_void()));
        ASSERT_SUCCEEDED(m_dxgi_factory->EnumAdapters1(adapter_index, m_dxgi_adapter.put()));

        DXGI_ADAPTER_DESC adapter_desc{};
        m_dxgi_adapter->GetDesc(&adapter_desc);
        
        m_adapter_desc.name = wsts(adapter_desc.Description);
        m_adapter_desc.local_memory = adapter_desc.DedicatedVideoMemory;
        m_adapter_desc.vendor_id = adapter_desc.VendorId;
        m_adapter_desc.device_id = adapter_desc.DeviceId;

        ASSERT_SUCCEEDED(D3D12CreateDevice(m_dxgi_adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), m_d3d12_device.put_void()));
    
        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        ASSERT_SUCCEEDED(m_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_cmd_queues[CommandBufferType::Graphics].put_void()));

        m_cmd_queues[CommandListType::Graphics] = std::make_unique<D3DCommandQueue>(m_d3d12_device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT);
        m_cmd_queues[CommandListType::Copy] = std::make_unique<D3DCommandQueue>(m_d3d12_device.get(), D3D12_COMMAND_LIST_TYPE_COPY);
        m_cmd_queues[CommandListType::Compute] = std::make_unique<D3DCommandQueue>(m_d3d12_device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE);
    }

    void wait(const CommandBufferType type, Fence* fence, const uint64 value) override {
        ASSERT_SUCCEEDED(m_cmd_queues[type]->Wait(static_cast<D3DFence*>(fence)., value));
    }

    void signal(const CommandBufferType type, Fence* fence, const uint64 value) override {
        m_cmd_queues[CommandBufferType::Graphics]->
    }

    void client_wait(Fence* fence, const uint64 value) {

    }

    void client_signal() {
        
    }

    void execute_command_buffers(const std::vector<CommandBuffer*>& cmd_buffers) override {

    }

    ID3D12Device4* get_d3d12_device() { return m_d3d12_device.get(); }

private:

    winrt::com_ptr<IDXGIFactory4> m_dxgi_factory;
    winrt::com_ptr<ID3D12Debug1> m_d3d12_debug;
    winrt::com_ptr<IDXGIAdapter1> m_dxgi_adapter;
    winrt::com_ptr<ID3D12Device4> m_d3d12_device;

    AdapterDesc m_adapter_desc;

    std::map<CommandBufferType, winrt::com_ptr<ID3D12CommandQueue>> m_cmd_queues;
};

}