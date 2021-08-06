// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DSwapchain : public Swapchain {
public:

    D3DSwapchain(winrt::com_ptr<IDXGIFactory4>& factory, winrt::com_ptr<ID3D12CommandQueue>& queue, HWND hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) :
        m_dxgi_factory(factory), m_d3d12_command_queue(queue) {
        
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
        m_dxgi_factory.get()->CreateSwapChainForHwnd(m_d3d12_command_queue.get().get(), reinterpret_cast<HWND>(hwnd), &swapchain_desc, nullptr, nullptr, temp_swapchain.put());
        temp_swapchain.as(m_dxgi_swapchain);
    }

    winrt::com_ptr<IDXGISwapChain4>& get_swapchain() { return m_dxgi_swapchain; }

private:

    std::reference_wrapper<winrt::com_ptr<IDXGIFactory4>> m_dxgi_factory;
    std::reference_wrapper<winrt::com_ptr<ID3D12CommandQueue>> m_d3d12_command_queue;

    winrt::com_ptr<IDXGISwapChain4> m_dxgi_swapchain;
};

}