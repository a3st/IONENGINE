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
        THROW_IF_FAILED(CreateDXGIFactory2(flags, __uuidof(IDXGIFactory4), m_dxgi_factory.put_void()));
        THROW_IF_FAILED(m_dxgi_factory->EnumAdapters1(adapter_index, m_dxgi_adapter.put()));

        DXGI_ADAPTER_DESC adapter_desc{};
        m_dxgi_adapter->GetDesc(&adapter_desc);
        
        m_adapter_desc.name = wsts(adapter_desc.Description);
        m_adapter_desc.local_memory = adapter_desc.DedicatedVideoMemory;
        m_adapter_desc.vendor_id = adapter_desc.VendorId;
        m_adapter_desc.device_id = adapter_desc.DeviceId;

        THROW_IF_FAILED(D3D12CreateDevice(m_dxgi_adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), m_d3d12_device.put_void()));
    
        m_command_queues[CommandBufferType::Graphics] = nullptr;
        m_command_queues[CommandBufferType::Copy] = nullptr;
        m_command_queues[CommandBufferType::Compute] = nullptr;

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandBufferType::Graphics].put_void()));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandBufferType::Copy].put_void()));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandBufferType::Compute].put_void()));
    }

    void wait(const CommandBufferType type, Fence* fence, const uint64 value) override {
        THROW_IF_FAILED(m_command_queues[type]->Wait(static_cast<D3DFence*>(fence)->get_d3d12_fence(), value));
    }

    void signal(const CommandBufferType type, Fence* fence, const uint64 value) override {
        THROW_IF_FAILED(m_command_queues[type]->Signal(static_cast<D3DFence*>(fence)->get_d3d12_fence(), value));
    }

    void execute_command_buffers(const std::vector<CommandBuffer*>& cmd_buffers) override {

    }

    std::unique_ptr<Buffer> create_buffer(const BufferType type, const BufferDesc& buffer_desc) override {
        return std::make_unique<D3DBuffer>(m_d3d12_device.get(), type, buffer_desc);
    }

    std::unique_ptr<Swapchain> create_swapchain(const SwapchainDesc& swapchain_desc) override {
        return std::make_unique<D3DSwapchain>(m_dxgi_factory.get(), m_d3d12_device.get(), m_command_queues[CommandBufferType::Graphics].get(), swapchain_desc);
    }

    std::unique_ptr<Memory> create_memory(const MemoryType memory_type, const usize size, const uint32 aligment, const BufferFlags buffer_flags) override {
        return std::make_unique<D3DMemory>(m_d3d12_device.get(), memory_type, size, aligment, buffer_flags);
    }

    const AdapterDesc& get_adapter_desc() const override { return m_adapter_desc; }

    ID3D12Device4* get_d3d12_device() { return m_d3d12_device.get(); }

private:

    winrt::com_ptr<IDXGIFactory4> m_dxgi_factory;
    winrt::com_ptr<ID3D12Debug1> m_d3d12_debug;
    winrt::com_ptr<IDXGIAdapter1> m_dxgi_adapter;
    winrt::com_ptr<ID3D12Device4> m_d3d12_device;

    AdapterDesc m_adapter_desc;

    std::map<CommandBufferType, winrt::com_ptr<ID3D12CommandQueue>> m_command_queues;
};

std::unique_ptr<Device> create_unique_device(const uint32 adapter_index) {
    return std::make_unique<D3DDevice>(adapter_index);
}

}