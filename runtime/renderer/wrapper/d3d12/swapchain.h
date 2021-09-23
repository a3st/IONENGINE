// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DSwapchain : public Swapchain {
public:

    D3DSwapchain(IDXGIFactory4* d3d12_factory, ID3D12Device4* d3d12_device, ID3D12CommandQueue* d3d12_command_queue, const SwapchainDesc& swapchain_desc) 
        : m_dxgi_factory(d3d12_factory), m_d3d12_device(d3d12_device), m_d3d12_command_queue(d3d12_command_queue), m_desc(swapchain_desc) {
        
        DXGI_SWAP_CHAIN_DESC1 dxgi_swapchain_desc{};
        dxgi_swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        dxgi_swapchain_desc.Width = swapchain_desc.width;
        dxgi_swapchain_desc.Height = swapchain_desc.height;
        dxgi_swapchain_desc.BufferCount = swapchain_desc.buffer_count;
        dxgi_swapchain_desc.SampleDesc.Count = 1;
        dxgi_swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        dxgi_swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        dxgi_swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        winrt::com_ptr<IDXGISwapChain1> tmp_swapchain;
        THROW_IF_FAILED(
            m_dxgi_factory->CreateSwapChainForHwnd(
                d3d12_command_queue, 
                reinterpret_cast<HWND>(swapchain_desc.hwnd), 
                &dxgi_swapchain_desc, 
                nullptr, 
                nullptr, 
                tmp_swapchain.put()
            )
        );
        tmp_swapchain.as(m_dxgi_swapchain);

        for(uint32 i = 0; i < swapchain_desc.buffer_count; ++i) {
            winrt::com_ptr<ID3D12Resource> resource;
            THROW_IF_FAILED(m_dxgi_swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));
            m_buffers.emplace_back(std::make_unique<D3DBuffer>(m_d3d12_device, BufferType::Texture, resource, BufferFlags::RenderTarget));
        }
    }

    Buffer* get_buffer(const uint32 buffer_index) override { return m_buffers[buffer_index].get(); }
    uint32 get_buffer_index() const override { return m_dxgi_swapchain->GetCurrentBackBufferIndex(); }

    uint32 next_buffer(Fence* fence, const uint64 signal_value) override {
        uint32 buffer_index = m_dxgi_swapchain->GetCurrentBackBufferIndex();
        THROW_IF_FAILED(m_d3d12_command_queue->Signal(static_cast<D3DFence*>(fence)->get_d3d12_fence(), signal_value));
        return buffer_index;
    }

    void present(Fence* fence, const uint64 wait_value) override {
        THROW_IF_FAILED(m_d3d12_command_queue->Wait(static_cast<D3DFence*>(fence)->get_d3d12_fence(), wait_value));
        THROW_IF_FAILED(m_dxgi_swapchain->Present(0, 0));
    }

    IDXGISwapChain4* get_dxgi_swapchain() { return m_dxgi_swapchain.get(); }

private:

    IDXGIFactory4* m_dxgi_factory;
    ID3D12Device4* m_d3d12_device;
    ID3D12CommandQueue* m_d3d12_command_queue;

    winrt::com_ptr<IDXGISwapChain4> m_dxgi_swapchain;

    std::vector<std::unique_ptr<D3DBuffer>> m_buffers;

    SwapchainDesc m_desc;
};

}