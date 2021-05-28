// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Swapchain {
public:

    Swapchain(const Instance& instance, const Device& device, void* hwnd, const DXGI_FORMAT buffer_format, const uint32 buffer_count) : 
        m_buffer_count(buffer_count) {
        
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.BufferCount = m_buffer_count;
        swapchain_desc.Format = buffer_format;
        swapchain_desc.Width = 1;
        swapchain_desc.Height = 1;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        ComPtr<IDXGISwapChain1> temp_swapchain;
        instance.m_factory->CreateSwapChainForHwnd(device.m_queues.direct.Get(), reinterpret_cast<HWND>(hwnd), &swapchain_desc, nullptr, nullptr, temp_swapchain.GetAddressOf());
        temp_swapchain.As(&m_swapchain);

        m_image_views.resize(m_buffer_count);

        for(uint32 i = 0; i < m_image_views.size(); ++i) {
            m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_image_views[i].m_resource));
        }
    }

    void resize(const uint32 width, const uint32 height) {
        DXGI_SWAP_CHAIN_DESC1 prev_swapchain_desc = {};
        m_swapchain->GetDesc1(&prev_swapchain_desc);

        m_image_views.clear();
        m_swapchain->ResizeBuffers(m_buffer_count, width, height, prev_swapchain_desc.Format, prev_swapchain_desc.Flags);

        for(uint32 i = 0; i < m_image_views.size(); ++i) {
            m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_image_views[i].m_resource));
        }
    }

    const std::vector<ImageView>& get_image_views() const {
        return m_image_views;
    }

private:

    ComPtr<IDXGISwapChain4> m_swapchain;
    std::vector<ImageView> m_image_views;

    uint32 m_buffer_count;
};

}