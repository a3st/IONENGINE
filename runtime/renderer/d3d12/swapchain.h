// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Swapchain {
public:

    Swapchain(const Instance& instance, const Device& device, void* hwnd, const ImageFormat buffer_format, const uint32 buffer_count) : 
        m_buffer_count(buffer_count) {
        
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.BufferCount = m_buffer_count;
        swapchain_desc.Format = static_cast<DXGI_FORMAT>(buffer_format);
        swapchain_desc.Width = 1;
        swapchain_desc.Height = 1;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        ComPtr<IDXGISwapChain1> temp_swapchain;
        instance.m_factory->CreateSwapChainForHwnd(device.m_queues.direct.Get(), reinterpret_cast<HWND>(hwnd), &swapchain_desc, nullptr, nullptr, temp_swapchain.GetAddressOf());
        temp_swapchain.As(&m_swapchain);

        m_render_targets.resize(m_buffer_count);

        for(uint32 i = 0; i < m_render_targets.size(); ++i) {
            m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_render_targets[i].m_resource));
        }
    }

    void resize(const uint32 width, const uint32 height) {
        DXGI_SWAP_CHAIN_DESC1 prev_swapchain_desc = {};
        m_swapchain->GetDesc1(&prev_swapchain_desc);

        m_render_targets.clear();
        m_swapchain->ResizeBuffers(m_buffer_count, width, height, prev_swapchain_desc.Format, prev_swapchain_desc.Flags);

        for(uint32 i = 0; i < m_render_targets.size(); ++i) {
            m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_render_targets[i].m_resource));
        }
    }

    const std::vector<RenderTarget>& get_render_targets() const {
        return m_render_targets;
    }

private:

    ComPtr<IDXGISwapChain4> m_swapchain;
    std::vector<RenderTarget> m_render_targets;

    uint32 m_buffer_count;
};

}