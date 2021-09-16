// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class D3DSwapchain : public Swapchain {
public:

    D3DSwapchain(winrt::com_ptr<IDXGIFactory4>& factory, winrt::com_ptr<ID3D12Device4>& device, winrt::com_ptr<ID3D12CommandQueue>& queue, HWND hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) 
        : m_dxgi_factory(factory), m_d3d12_device(device), m_d3d12_command_queue(queue) {
        
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
        swapchain_desc.BufferCount = buffer_count;
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.Width = width;
        swapchain_desc.Height = height;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        winrt::com_ptr<IDXGISwapChain1> temp_swapchain;
        ASSERT_SUCCEEDED(m_dxgi_factory.get()->CreateSwapChainForHwnd(m_d3d12_command_queue.get().get(), reinterpret_cast<HWND>(hwnd), &swapchain_desc, nullptr, nullptr, temp_swapchain.put()));
        temp_swapchain.as(m_dxgi_swapchain);

        for(uint32 i = 0; i < buffer_count; ++i) {
            
            winrt::com_ptr<ID3D12Resource> d3d12_resource;
            ASSERT_SUCCEEDED(m_dxgi_swapchain->GetBuffer(i, __uuidof(ID3D12Resource), d3d12_resource.put_void()));
            m_back_buffers.emplace_back(std::make_unique<D3DResource>(m_d3d12_device, ResourceType::Texture, d3d12_resource, ResourceFlags::RenderTarget));
        }
    }

    Resource& get_back_buffer(const uint32 buffer_index) override { return *m_back_buffers[buffer_index]; }

    uint32 next_buffer(Fence& fence, const uint64 signal_value) override {
        uint32 buffer_index = m_dxgi_swapchain->GetCurrentBackBufferIndex();
        ASSERT_SUCCEEDED(m_d3d12_command_queue.get()->Signal(static_cast<D3DFence&>(fence).get_d3d12_fence().get(), signal_value));
        return buffer_index;
    }

    void present(Fence& fence, const uint64 wait_value) override {
        ASSERT_SUCCEEDED(m_d3d12_command_queue.get()->Wait(static_cast<D3DFence&>(fence).get_d3d12_fence().get(), wait_value));

        ASSERT_SUCCEEDED(m_dxgi_swapchain->Present(0, 0));
    }
    
    uint32 get_back_buffer_index() const { return m_dxgi_swapchain->GetCurrentBackBufferIndex(); }

    winrt::com_ptr<IDXGISwapChain4>& get_d3d12_swapchain() { return m_dxgi_swapchain; }

private:

    std::reference_wrapper<winrt::com_ptr<IDXGIFactory4>> m_dxgi_factory;
    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_d3d12_device;
    std::reference_wrapper<winrt::com_ptr<ID3D12CommandQueue>> m_d3d12_command_queue;

    winrt::com_ptr<IDXGISwapChain4> m_dxgi_swapchain;

    std::vector<std::unique_ptr<D3DResource>> m_back_buffers;
};

}