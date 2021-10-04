// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

using namespace memory_literals;







template<>
class Device<backend::d3d12> {

    friend const AdapterDesc& get_adapter_desc<backend::d3d12>(Device<backend::d3d12>* device);
    friend uint32 get_swapchain_buffer_index<backend::d3d12>(Device<backend::d3d12>* device);
    friend uint32 get_swapchain_buffer_size<backend::d3d12>(Device<backend::d3d12>* device);

public:

    Device(const uint32 adapter_index, void* window, const uint32 width, const uint32 height, const uint32 buffer_count, const uint32 multisample_count) {

        assert(window && "pointer to window is null");

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
        
        m_adapter_desc.name = lib::wsts(adapter_desc.Description);
        m_adapter_desc.local_memory = adapter_desc.DedicatedVideoMemory;
        m_adapter_desc.vendor_id = adapter_desc.VendorId;
        m_adapter_desc.device_id = adapter_desc.DeviceId;

        THROW_IF_FAILED(D3D12CreateDevice(m_dxgi_adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), m_d3d12_device.put_void()));

        D3D12_COMMAND_QUEUE_DESC queue_desc{};
        queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandListType::Graphics].put_void()));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandListType::Copy].put_void()));

        queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        THROW_IF_FAILED(m_d3d12_device->CreateCommandQueue(&queue_desc, __uuidof(ID3D12CommandQueue), m_command_queues[CommandListType::Compute].put_void()));
    
        MemoryAllocatorWrapper<backend::d3d12>::initialize(m_d3d12_device.get(), 32_mb, 512_mb, 128_mb);
        DescriptorAllocatorWrapper<backend::d3d12>::initialize(m_d3d12_device.get());

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
    }

    void wait(const CommandListType command_list_type, Fence<backend::d3d12>* fence, const uint64 value) {

        assert(fence && "pointer to fence is null");

        THROW_IF_FAILED(m_command_queues[command_list_type]->Wait(fence->get_d3d12_fence(), value));
    }

    void signal(const CommandListType command_list_type, Fence<backend::d3d12>* fence, const uint64 value) {

        assert(fence && "pointer to fence is null");

        THROW_IF_FAILED(m_command_queues[command_list_type]->Signal(fence->get_d3d12_fence(), value));
    }

    /*void execute_command_lists(const CommandListType command_list_type, const std::vector<CommandList*>& command_lists) override {
        
        std::vector<ID3D12CommandList*> d3d12_command_lists;

        for(auto command_list : command_lists) {
            auto d3d_command_list = static_cast<D3DCommandList*>(command_list);
            d3d12_command_lists.emplace_back(d3d_command_list->get_d3d12_command_list());
        }

        if(!d3d12_command_lists.empty()) {
            m_command_queues[command_list_type]->ExecuteCommandLists(static_cast<uint32>(d3d12_command_lists.size()), d3d12_command_lists.data());
        }
    }*/

    std::unique_ptr<Resource<backend::d3d12>> create_resource(const ResourceType resource_type, const MemoryType memory_type, const ResourceDesc& resource_desc) {
        return std::make_unique<Resource<backend::d3d12>>(m_d3d12_device.get(), memory_type, resource_type, resource_desc);
    }

    std::unique_ptr<Resource<backend::d3d12>> create_resource(const SamplerDesc& sampler_desc) {
        return std::make_unique<Resource<backend::d3d12>>(m_d3d12_device.get(), sampler_desc);
    }

    std::unique_ptr<Fence<backend::d3d12>> create_fence(const uint64 initial_value) {
        return std::make_unique<Fence<backend::d3d12>>(m_d3d12_device.get(), initial_value);
    }

    std::unique_ptr<View<backend::d3d12>> create_view(const ViewType view_type, Resource<backend::d3d12>* resource, const ViewDesc& view_desc) {

        assert(resource && "pointer to resource is null");

        return std::make_unique<View<backend::d3d12>>(m_d3d12_device.get(), view_type, resource, view_desc);
    }

    void present() {
        THROW_IF_FAILED(m_dxgi_swapchain->Present(0, 0));
    }

    std::unique_ptr<Resource<backend::d3d12>> create_swapchain_resource(const uint32 buffer_index) { 
        
        winrt::com_ptr<ID3D12Resource> resource;
        THROW_IF_FAILED(m_dxgi_swapchain->GetBuffer(buffer_index, __uuidof(ID3D12Resource), resource.put_void()));
            
        return std::make_unique<Resource<backend::d3d12>>(m_d3d12_device.get(), ResourceType::Texture, resource, ResourceFlags::RenderTarget);
    }

    std::unique_ptr<BindingSetLayout<backend::d3d12>> create_binding_set_layout(const std::vector<BindingSetInputDesc>& bindings) {
        return std::make_unique<BindingSetLayout<backend::d3d12>>(m_d3d12_device.get(), bindings);
    }

    std::unique_ptr<BindingSet<backend::d3d12>> create_binding_set(BindingSetLayout<backend::d3d12>* layout) {

        assert(layout && "pointer to layout is null");

        return std::make_unique<BindingSet<backend::d3d12>>(m_d3d12_device.get(), layout);
    }

    std::unique_ptr<RenderPass<backend::d3d12>> create_render_pass(const RenderPassDesc& render_pass_desc) {
        return std::make_unique<RenderPass<backend::d3d12>>(render_pass_desc);
    }

    std::unique_ptr<FrameBuffer<backend::d3d12>> create_frame_buffer(const FrameBufferDesc<backend::d3d12>& frame_buffer_desc) {
        return std::make_unique<FrameBuffer<backend::d3d12>>(frame_buffer_desc);
    }

    /*

    std::unique_ptr<Pipeline> create_pipeline(const GraphicsPipelineDesc& pipeline_desc) override {
        return std::make_unique<D3DPipeline>(m_d3d12_device.get(), pipeline_desc);
    }

    std::unique_ptr<Pipeline> create_pipeline(const ComputePipelineDesc& pipeline_desc) override {
        return std::make_unique<D3DPipeline>(m_d3d12_device.get(), pipeline_desc);
    }

    std::unique_ptr<CommandList> create_command_list(const CommandListType command_list_type) override {
        return std::make_unique<D3DCommandList>(m_d3d12_device.get(), command_list_type);
    }

    */

private:

    winrt::com_ptr<IDXGIFactory4> m_dxgi_factory;
    winrt::com_ptr<ID3D12Debug1> m_d3d12_debug;
    winrt::com_ptr<IDXGIAdapter1> m_dxgi_adapter;
    winrt::com_ptr<ID3D12Device4> m_d3d12_device;
    winrt::com_ptr<IDXGISwapChain4> m_dxgi_swapchain;

    AdapterDesc m_adapter_desc;

    std::map<CommandListType, winrt::com_ptr<ID3D12CommandQueue>> m_command_queues;
};

template<class B = backend::base>
inline std::unique_ptr<Device<B>> create_device(
    const uint32 adapter_index, 
    void* window, 
    const uint32 width, 
    const uint32 height, 
    const uint32 buffer_count, 
    const uint32 multisample_count
) {

    return std::make_unique<Device<B>>(adapter_index, window, width, height, buffer_count, multisample_count);
}

template<>
inline const AdapterDesc& get_adapter_desc<backend::d3d12>(Device<backend::d3d12>* device) {
    return device->m_adapter_desc;
}

template<>
inline uint32 get_swapchain_buffer_index<backend::d3d12>(Device<backend::d3d12>* device) { 
    return device->m_dxgi_swapchain->GetCurrentBackBufferIndex(); 
}

template<>
inline uint32 get_swapchain_buffer_size<backend::d3d12>(Device<backend::d3d12>* device) { 
        
    DXGI_SWAP_CHAIN_DESC swapchain_desc{};
    device->m_dxgi_swapchain->GetDesc(&swapchain_desc);
    return swapchain_desc.BufferCount;
}

}