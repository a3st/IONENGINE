// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DDevice : public Device {
public:

    D3DDevice(const ComPtr<IDXGIFactory4>& factory, const ComPtr<IDXGIAdapter1>& adapter) : m_dxgi_factory(factory), m_dxgi_adapter(adapter) {

        ASSERT_SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3d12_device)));

        m_command_queues[CommandListType::Graphics] = std::make_shared<D3DCommandQueue>(m_d3d12_device, CommandListType::Graphics);
        m_command_queues[CommandListType::Copy] = std::make_shared<D3DCommandQueue>(m_d3d12_device, CommandListType::Copy);
        m_command_queues[CommandListType::Compute] = std::make_shared<D3DCommandQueue>(m_d3d12_device, CommandListType::Compute);
    }

    const ComPtr<ID3D12Device4>& get_device() const { return m_d3d12_device; }
    
    std::shared_ptr<CommandQueue> get_command_queue(const CommandListType type) override {
        return m_command_queues[type];
    }

    std::unique_ptr<Swapchain> create_swapchain(void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) override {
        return std::make_unique<D3DSwapchain>(m_dxgi_factory, m_command_queues[CommandListType::Graphics]->get_command_queue(), reinterpret_cast<HWND>(hwnd), width, height, buffer_count);
    }

    std::unique_ptr<Shader> create_shader(const std::vector<byte>& blob) override {
        return std::make_unique<D3DShader>(m_d3d12_device, blob);
    }

    std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) override {
        return std::make_unique<D3DDescriptorSetLayout>(m_d3d12_device, bindings);
    }

private:

    const ComPtr<IDXGIAdapter1>& m_dxgi_adapter;
    const ComPtr<IDXGIFactory4>& m_dxgi_factory;

    ComPtr<ID3D12Device4> m_d3d12_device;
    
    std::map<CommandListType, std::shared_ptr<D3DCommandQueue>> m_command_queues;
};

}