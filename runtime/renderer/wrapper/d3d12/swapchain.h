// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DSwapchain : public Swapchain {
public:

    D3DSwapchain(IDXGIFactory4* factory, IDXGIDevice1* device, ID3D12CommandQueue* command_queue, const SwapchainDesc& swapchain_desc) 
        : m_dxgi_factory(factory), m_d3d12_device(device), m_d3d12_command_queue(command_queue) {
        
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
        ASSERT_SUCCEEDED(m_dxgi_factory->CreateSwapChainForHwnd(
                m_queues[CommandListType::Graphics]->get_d3d12_queue().get(), 
                reinterpret_cast<HWND>(swapchain_desc.hwnd), 
                &dxgi_swapchain_desc, 
                nullptr, 
                nullptr, 
                tmp_swapchain.put()
            )
        );
        tmp_swapchain.as(m_dxgi_swapchain);

        for(uint32 i = 0; i < buffer_count; ++i) {
            winrt::com_ptr<ID3D12Resource> resource;
            ASSERT_SUCCEEDED(m_dxgi_swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));
            m_back_buffers.emplace_back(std::make_unique<D3DResource>(m_d3d12_device.get(), ResourceType::Texture, resource, ResourceFlags::RenderTarget));
        }
    }

    Resource* get_back_buffer(const uint32 buffer_index) override { return m_back_buffers[buffer_index]; }
    uint32 get_back_buffer_index() const override { return m_dxgi_swapchain->GetCurrentBackBufferIndex(); }

    uint32 next_buffer(Fence* fence, const uint64 signal_value) override {
        uint32 buffer_index = m_dxgi_swapchain->GetCurrentBackBufferIndex();
        ASSERT_SUCCEEDED(m_d3d12_command_queue->Signal(static_cast<D3DFence*>(fence)->get_d3d12_fence().get(), signal_value));
        return buffer_index;
    }

    void present(Fence* fence, const uint64 wait_value) override {
        ASSERT_SUCCEEDED(m_d3d12_command_queue->Wait(static_cast<D3DFence*>(fence)->get_d3d12_fence().get(), wait_value));
        ASSERT_SUCCEEDED(m_dxgi_swapchain->Present(0, 0));
    }

    IDXGISwapChain4* get_dxgi_swapchain() { return m_dxgi_swapchain.get(); }

private:

    const IDXGIFactory4* m_dxgi_factory;
    const ID3D12Device4* m_d3d12_device;
    const ID3D12CommandQueue* m_d3d12_command_queue;

    winrt::com_ptr<IDXGISwapChain4> m_dxgi_swapchain;

    std::vector<std::unique_ptr<D3DResource>> m_back_buffers;
};

}