// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Swapchain {
public:

    Swapchain(Device& device, void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) : m_buffer_count(buffer_count) {

#ifdef VK_USE_PLATFORM_WIN32_KHR

        vk::Win32SurfaceCreateInfoKHR surface_info{};

        surface_info
            .setHwnd(reinterpret_cast<HWND>(hwnd))
            .setHinstance(GetModuleHandle(nullptr));

        m_surface = device.m_instance->createWin32SurfaceKHRUnique(surface_info);

#endif

        // Checks if graphics queue supports present
        vk::Bool32 result = device.m_adapter.getSurfaceSupportKHR(device.m_queues.family_graphics, m_surface.get());
        if(!result) {
            throw std::runtime_error("Graphics queue not supported a present");
        }

        vk::SurfaceCapabilitiesKHR surface_capabilities = device.m_adapter.getSurfaceCapabilitiesKHR(m_surface.get());
        auto surface_formats = device.m_adapter.getSurfaceFormatsKHR(m_surface.get());
        auto present_modes = device.m_adapter.getSurfacePresentModesKHR(m_surface.get());

        vk::SwapchainCreateInfoKHR swapchain_info{};

        swapchain_info
            .setSurface(m_surface.get())
            .setMinImageCount(m_buffer_count)
            .setImageFormat(surface_formats[0].format)
            .setImageColorSpace(surface_formats[0].colorSpace)
            .setImageExtent(surface_capabilities.currentExtent)
            .setImageArrayLayers(1)
            .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
            .setImageSharingMode(vk::SharingMode::eExclusive)
            .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
            .setPresentMode(vk::PresentModeKHR::eFifo)
            .setPreTransform(surface_capabilities.currentTransform)
            .setOldSwapchain(nullptr)
            .setClipped(VK_TRUE);

        m_swapchain = device.m_device->createSwapchainKHRUnique(swapchain_info);
        auto images = device.m_device->getSwapchainImagesKHR(m_swapchain.get());

        for(uint32 i = 0; i < images.size(); ++i) {
            
        }
    }

    Swapchain(const Swapchain&) = delete;

    Swapchain(Swapchain&& rhs) noexcept {

        m_surface.swap(rhs.m_surface);
        m_swapchain.swap(rhs.m_swapchain);
    }

    Swapchain& operator=(const Swapchain&) = delete;

    Swapchain& operator=(Swapchain&& rhs) noexcept {

        m_surface.swap(rhs.m_surface);
        m_swapchain.swap(rhs.m_swapchain);
        return *this;
    }

private:

    vk::UniqueSurfaceKHR m_surface;
    vk::UniqueSwapchainKHR m_swapchain;

    uint32 m_buffer_count;
};

}