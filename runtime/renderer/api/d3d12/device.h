// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class D3DDevice : public Device {
public:

    D3DDevice(winrt::com_ptr<IDXGIFactory4>& factory, winrt::com_ptr<IDXGIAdapter1>& adapter) : m_dxgi_factory(factory), m_dxgi_adapter(adapter) {

        ASSERT_SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), m_d3d12_device.put_void()));

        m_command_queues[CommandListType::Graphics] = std::make_unique<D3DCommandQueue>(m_d3d12_device, CommandListType::Graphics);
        m_command_queues[CommandListType::Copy] = std::make_unique<D3DCommandQueue>(m_d3d12_device, CommandListType::Copy);
        m_command_queues[CommandListType::Compute] = std::make_unique<D3DCommandQueue>(m_d3d12_device, CommandListType::Compute);
    }
    
    CommandQueue& get_command_queue(const CommandListType list_type) override {
        return *m_command_queues[list_type];
    }

    std::unique_ptr<Swapchain> create_swapchain(void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) override {
        return std::make_unique<D3DSwapchain>(m_dxgi_factory, m_d3d12_device, m_command_queues[CommandListType::Graphics]->get_d3d12_command_queue(), reinterpret_cast<HWND>(hwnd), width, height, buffer_count);
    }

    std::unique_ptr<Shader> create_shader(const ShaderType shader_type, const std::vector<byte>& shader_blob) override {
        return std::make_unique<D3DShader>(m_d3d12_device, shader_type, shader_blob);
    }

    std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) override {
        return std::make_unique<D3DDescriptorSetLayout>(m_d3d12_device, bindings);
    }

    std::unique_ptr<RenderPass> create_render_pass(const RenderPassDesc& render_pass_desc) override {
        return std::make_unique<D3DRenderPass>(m_d3d12_device, render_pass_desc);
    }

    std::unique_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineDesc& pipeline_desc) override {
        return std::make_unique<D3DPipeline>(m_d3d12_device, pipeline_desc);
    }

    std::unique_ptr<Resource> create_resource(const ResourceType resource_type, const std::variant<ResourceDesc, SamplerDesc>& resource_desc) override {
        return std::make_unique<D3DResource>(m_d3d12_device, resource_type, resource_desc);
    }

    std::unique_ptr<Memory> allocate_memory(const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags flags) override {
        return std::make_unique<D3DMemory>(m_d3d12_device, memory_type, size, alignment, flags);
    }

    std::unique_ptr<DescriptorPool> create_descriptor_pool(const std::vector<DescriptorPoolSize>& pool_sizes) override {
        return std::make_unique<D3DDescriptorPool>(m_d3d12_device, pool_sizes);
    }

    std::unique_ptr<Fence> create_fence(const uint64 initial_value) override {
        return std::make_unique<D3DFence>(m_d3d12_device, initial_value);
    }

    std::unique_ptr<CommandList> create_command_list(const CommandListType list_type) override {
        return std::make_unique<D3DCommandList>(m_d3d12_device, list_type);
    }

    std::unique_ptr<View> create_view(DescriptorPool& descriptor_pool, Resource& resource, const ViewDesc& view_desc) override {
        return std::make_unique<D3DView>(m_d3d12_device, static_cast<D3DDescriptorPool&>(descriptor_pool), static_cast<D3DResource&>(resource), view_desc);
    }

    std::unique_ptr<FrameBuffer> create_frame_buffer(const FrameBufferDesc& frame_buffer_desc) override {
        return std::make_unique<D3DFrameBuffer>(m_d3d12_device, frame_buffer_desc);
    }

    winrt::com_ptr<ID3D12Device4>& get_d3d12_device() { return m_d3d12_device; }

private:

    std::reference_wrapper<winrt::com_ptr<IDXGIAdapter1>> m_dxgi_adapter;
    std::reference_wrapper<winrt::com_ptr<IDXGIFactory4>> m_dxgi_factory;

    winrt::com_ptr<ID3D12Device4> m_d3d12_device;
    
    std::map<CommandListType, std::unique_ptr<D3DCommandQueue>> m_command_queues;
};

}