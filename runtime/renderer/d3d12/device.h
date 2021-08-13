// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DDevice : public Device {
public:

    D3DDevice(winrt::com_ptr<IDXGIFactory4>& factory, winrt::com_ptr<IDXGIAdapter1>& adapter) : m_dxgi_factory(factory), m_dxgi_adapter(adapter) {

        ASSERT_SUCCEEDED(D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device4), m_d3d12_device.put_void()));

        m_command_queues[CommandListType::Graphics] = std::make_unique<D3DCommandQueue>(m_d3d12_device, CommandListType::Graphics);
        m_command_queues[CommandListType::Copy] = std::make_unique<D3DCommandQueue>(m_d3d12_device, CommandListType::Copy);
        m_command_queues[CommandListType::Compute] = std::make_unique<D3DCommandQueue>(m_d3d12_device, CommandListType::Compute);
    }

    winrt::com_ptr<ID3D12Device4>& get_device() { return m_d3d12_device; }
    
    CommandQueue& get_command_queue(const CommandListType type) override {
        return *m_command_queues[type];
    }

    std::unique_ptr<Swapchain> create_swapchain(void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) override {
        return std::make_unique<D3DSwapchain>(m_dxgi_factory, m_d3d12_device, m_command_queues[CommandListType::Graphics]->get_command_queue(), reinterpret_cast<HWND>(hwnd), width, height, buffer_count);
    }

    std::unique_ptr<Shader> create_shader(const std::vector<byte>& blob) override {
        return std::make_unique<D3DShader>(m_d3d12_device, blob);
    }

    std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) override {
        return std::make_unique<D3DDescriptorSetLayout>(m_d3d12_device, bindings);
    }

    std::unique_ptr<RenderPass> create_render_pass(const RenderPassDesc& desc) override {
        return std::make_unique<D3DRenderPass>(m_d3d12_device, desc);
    }

    std::unique_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineDesc& desc) override {
        return std::make_unique<D3DPipeline>(m_d3d12_device, desc);
    }

    std::unique_ptr<Resource> create_buffer(const ResourceFlags flags, const usize buffer_size) override {
        
        usize align_size = buffer_size;
        if(flags & ResourceFlags::ConstantBuffer) {
            align_size = (buffer_size + 255) & ~255;
        }

        D3D12_RESOURCE_DESC resource_desc{};
        resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resource_desc.Alignment = 0;
        resource_desc.Width = align_size;
        resource_desc.Height = 1;
		resource_desc.DepthOrArraySize = 1;
		resource_desc.MipLevels = 1;
		resource_desc.Format = DXGI_FORMAT_UNKNOWN;
		resource_desc.SampleDesc.Count = 1;
		resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        if(flags & ResourceFlags::RenderTarget) {
            resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        } else if(flags & ResourceFlags::DepthStencil) {
            resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        } else if(flags & ResourceFlags::UnorderedAccess) {
            resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        return std::make_unique<D3DResource>(m_d3d12_device, resource_desc);
    }

    std::unique_ptr<Memory> allocate_memory(const MemoryType memory_type, const usize size, const uint32 align, const ResourceFlags memory_flags) override {
        return std::make_unique<D3DMemory>(m_d3d12_device, memory_type, size, align, memory_flags);
    }

    std::unique_ptr<DescriptorPool> create_descriptor_pool(const std::vector<DescriptorPoolSize>& sizes) override {
        return std::make_unique<D3DDescriptorPool>(m_d3d12_device, sizes);
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

private:

    std::reference_wrapper<winrt::com_ptr<IDXGIAdapter1>> m_dxgi_adapter;
    std::reference_wrapper<winrt::com_ptr<IDXGIFactory4>> m_dxgi_factory;

    winrt::com_ptr<ID3D12Device4> m_d3d12_device;
    
    std::map<CommandListType, std::unique_ptr<D3DCommandQueue>> m_command_queues;
};

}