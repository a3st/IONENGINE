// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Swapchain final {
public:

    Swapchain(Device& device, void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) :
        m_device(device), m_buffer_count(buffer_count) {
        
        DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
        swapchain_desc.BufferCount = m_buffer_count;
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.Width = width;
        swapchain_desc.Height = height;
        swapchain_desc.SampleDesc.Count = 1;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        ComPtr<IDXGISwapChain1> temp_swapchain;
        m_device.get().m_factory->CreateSwapChainForHwnd(m_device.get().m_queues.direct.Get(), reinterpret_cast<HWND>(hwnd), &swapchain_desc, nullptr, nullptr, temp_swapchain.GetAddressOf());
        temp_swapchain.As(&m_ptr);

        for(uint32 i = 0; i < m_buffer_count; ++i) {
            ID3D12Resource* resource;
            m_ptr->GetBuffer(i, IID_PPV_ARGS(&resource));
            m_attachments.emplace_back(m_device, resource);
        }
    }

    Swapchain(const Swapchain&) = delete;

    Swapchain(Swapchain&& rhs) noexcept : m_device(rhs.m_device) {

    }

    Swapchain& operator=(const Swapchain&) = delete;

    Swapchain& operator=(Swapchain&& rhs) noexcept {

        std::swap(m_device, rhs.m_device);

        m_ptr.Swap(rhs.m_ptr);

        std::swap(m_buffer_count, rhs.m_buffer_count);
        return *this;
    }

    const std::vector<FramebufferAttachment>& get_framebuffer_attachments() const { return m_attachments; }

private:

    std::reference_wrapper<Device> m_device;

    ComPtr<IDXGISwapChain4> m_ptr;

    uint32 m_buffer_count;
    std::vector<FramebufferAttachment> m_attachments;
};

}