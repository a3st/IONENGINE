// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

using namespace memory_literals;

class D3DDevice : public Device {
public:

    D3DDevice(const uint32 adapter_index, void* window, const uint32 width, const uint32 height, const uint32 buffer_count, const uint32 multisample_count) {

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
    
        m_command_queues[CommandListType::Graphics] = nullptr;
        m_command_queues[CommandListType::Copy] = nullptr;
        m_command_queues[CommandListType::Compute] = nullptr;

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandListType::Graphics].put_void()));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandListType::Copy].put_void()));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandListType::Compute].put_void()));
    
        DXGI_SWAP_CHAIN_DESC1 dxgi_swapchain_desc{};
        dxgi_swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dxgi_swapchain_desc.Width = width;
        dxgi_swapchain_desc.Height = height;
        dxgi_swapchain_desc.BufferCount = buffer_count;
        dxgi_swapchain_desc.SampleDesc.Count = multisample_count;
        dxgi_swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dxgi_swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        dxgi_swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        winrt::com_ptr<IDXGISwapChain1> tmp_swapchain;
        THROW_IF_FAILED(
            m_dxgi_factory->CreateSwapChainForHwnd(
                m_command_queues[CommandListType::Graphics].get(), 
                reinterpret_cast<HWND>(window), 
                &dxgi_swapchain_desc, 
                nullptr, 
                nullptr, 
                tmp_swapchain.put()
            )
        );
        tmp_swapchain.as(m_dxgi_swapchain);

        for(uint32 i = 0; i < buffer_count; ++i) {
            winrt::com_ptr<ID3D12Resource> resource;
            THROW_IF_FAILED(m_dxgi_swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));
            m_swapchain_buffers.emplace_back(std::make_unique<D3DResource>(m_d3d12_device.get(), ResourceType::Texture, resource, ResourceFlags::RenderTarget));
        }

        D3DMemoryAllocatorWrapper::initialize(m_d3d12_device.get(), 32_mb, 512_mb, 128_mb);
        D3DDescriptorAllocatorWrapper::initialize(m_d3d12_device.get());
    }

    void wait(const CommandListType command_list_type, Fence* fence, const uint64 value) override {
        THROW_IF_FAILED(m_command_queues[command_list_type]->Wait(static_cast<D3DFence*>(fence)->get_d3d12_fence(), value));
    }

    void signal(const CommandListType command_list_type, Fence* fence, const uint64 value) override {
        THROW_IF_FAILED(m_command_queues[command_list_type]->Signal(static_cast<D3DFence*>(fence)->get_d3d12_fence(), value));
    }

    void execute_command_buffers(const CommandListType command_list_type, const std::vector<CommandList*>& command_buffers) override {
        /*std::vector<ID3D12CommandList*> d3d12_command_lists;
        for(auto& command_buffer : command_buffers) {
            D3DCommandBuffer* d3d_command_buffer = static_cast<D3DCommandBuffer*>(command_buffer);
            d3d12_command_lists.emplace_back(d3d_command_buffer->get_d3d12_command_list().get());
        }
        if(!d3d12_command_lists.empty()) {
            m_command_queues[command_buffer_type]->ExecuteCommandLists(d3d12_command_lists.size(), d3d12_command_lists.data());
        }*/
    }

    std::unique_ptr<Resource> create_resource(const ResourceType resource_type, const MemoryType memory_type, const ResourceDesc& resource_desc) override {
        return std::make_unique<D3DResource>(m_d3d12_device.get(), memory_type, resource_type, resource_desc);
    }

    std::unique_ptr<Sampler> create_sampler(const SamplerDesc& sampler_desc) override {
        return std::make_unique<D3DSampler>(sampler_desc);
    }

    std::unique_ptr<Fence> create_fence(const uint64 initial_value) override {
        return std::make_unique<D3DFence>(m_d3d12_device.get(), initial_value);
    }

    std::unique_ptr<Pipeline> create_pipeline(const GraphicsPipelineDesc& pipeline_desc) override {
        return std::make_unique<D3DPipeline>(m_d3d12_device.get(), pipeline_desc);
    }

    std::unique_ptr<Pipeline> create_pipeline(const ComputePipelineDesc& pipeline_desc) override {
        return std::make_unique<D3DPipeline>(m_d3d12_device.get(), pipeline_desc);
    }

    std::unique_ptr<View> create_view(const ViewType view_type, Resource* resource, const ViewDesc& view_desc) override {
        return std::make_unique<D3DView>(m_d3d12_device.get(), view_type, static_cast<D3DResource*>(resource), view_desc);
    }

    std::unique_ptr<View> create_view(Sampler* sampler) override {
        return std::make_unique<D3DView>(m_d3d12_device.get(), static_cast<D3DSampler*>(sampler));
    }

    std::unique_ptr<DescriptorSet> create_descriptor_set(DescriptorSetLayout* layout) override {
        return std::make_unique<D3DDescriptorSet>(m_d3d12_device.get(), static_cast<D3DDescriptorSetLayout*>(layout));
    }

    void present() override {
        THROW_IF_FAILED(m_dxgi_swapchain->Present(0, 0));
    }

    Resource* get_swapchain_resource(const uint32 resource_index) override { return m_swapchain_buffers[resource_index].get(); }

    uint32 get_swapchain_resource_index() const override { return m_dxgi_swapchain->GetCurrentBackBufferIndex(); }

    const AdapterDesc& get_adapter_desc() const override { return m_adapter_desc; }

    ID3D12Device4* get_d3d12_device() { return m_d3d12_device.get(); }

private:

    winrt::com_ptr<IDXGIFactory4> m_dxgi_factory;
    winrt::com_ptr<ID3D12Debug1> m_d3d12_debug;
    winrt::com_ptr<IDXGIAdapter1> m_dxgi_adapter;
    winrt::com_ptr<ID3D12Device4> m_d3d12_device;
    winrt::com_ptr<IDXGISwapChain4> m_dxgi_swapchain;

    AdapterDesc m_adapter_desc;

    std::map<CommandListType, winrt::com_ptr<ID3D12CommandQueue>> m_command_queues;
    std::vector<std::unique_ptr<D3DResource>> m_swapchain_buffers;
};

std::unique_ptr<Device> create_unique_device(const uint32 adapter_index, void* window, const uint32 width, const uint32 height, const uint32 buffer_count, const uint32 multisample_count) {
    return std::make_unique<D3DDevice>(adapter_index, window, width, height, buffer_count, multisample_count);
}

}