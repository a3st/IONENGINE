// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Swapchain {
public:

    Swapchain(const Instance& instance, const Device& device, HWND hwnd, const DXGI_FORMAT buffer_format, const uint32 buffer_count) {
        
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc = {};
        swapchain_desc.BufferCount = buffer_count;
        swapchain_desc.Format = buffer_format;
        swapchain_desc.Width = 1;
        swapchain_desc.Height = 1;
        swapchain_desc.
        swapchain_desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

        ComPtr<IDXGISwapChain1> temp_swapchain;
        instance.m_factory->CreateSwapChainForHwnd(device.m_queues.direct.Get(), hwnd, &swapchain_desc, nullptr, nullptr, temp_swapchain.GetAddressOf());
        temp_swapchain.As(&m_swapchain);
   }

private:

    ComPtr<IDXGISwapChain4> m_swapchain;
};

}