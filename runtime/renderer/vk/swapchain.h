// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Swapchain final {
public:

    Swapchain(Instance& instance, Device& device, void* hwnd, const uint32 buffer_count) : m_instance(instance), m_device(device), m_buffer_count(buffer_count) {

#ifdef VK_USE_PLATFORM_WIN32_KHR

        vk::Win32SurfaceCreateInfoKHR surface_info{};

        surface_info
            .setHwnd(reinterpret_cast<HWND>(hwnd))
            .setHinstance(GetModuleHandle(nullptr));

        m_surface_handle = m_instance.get().get_handle()->createWin32SurfaceKHRUnique(surface_info);

#endif

        // Checks if graphics queue supports present
        vk::Bool32 result = m_device.get().get_adapter_handle().getSurfaceSupportKHR(m_device.get().get_graphics_family_index(), m_surface_handle.get());
        if(!result) {
            throw std::runtime_error("Graphics queue not supported a present");
        }

        vk::SurfaceCapabilitiesKHR surface_capabilities = m_device.get().get_adapter_handle().getSurfaceCapabilitiesKHR(m_surface_handle.get());
        auto surface_formats = m_device.get().get_adapter_handle().getSurfaceFormatsKHR(m_surface_handle.get());
        auto present_modes = m_device.get().get_adapter_handle().getSurfacePresentModesKHR(m_surface_handle.get());

        vk::SwapchainCreateInfoKHR swapchain_info{};

        swapchain_info
            .setSurface(m_surface_handle.get())
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

        m_handle = m_device.get().get_handle()->createSwapchainKHRUnique(swapchain_info);

        /*std::vector<VkImage> images;
        uint32 images_count = 0;
        vkGetSwapchainImagesKHR(m_device.get_handle(), m_swapchain_handle, &images_count, nullptr);
        
        if(images_count > 0) {
            images.resize(images_count);
            vkGetSwapchainImagesKHR(m_device.get_handle(), m_swapchain_handle, &images_count, images.data());
        } else {
            throw std::runtime_error("Images in swapchain not found");
        }

        for(uint32 i = 0; i < images_count; ++i) {
            //m_image_views.emplace_back(m_device, images[i], ImageViewType::Single2D, static_cast<ImageFormat>(surface_formats[0].format));
        }*/
    }

    void resize(const uint32 width, const uint32 height) {
        
    }

    //const std::vector<ImageView>& get_image_views() const { return m_image_views; }
    const vk::UniqueSurfaceKHR& get_surface_handle() const { return m_surface_handle; }
    const vk::UniqueSwapchainKHR& get_handle() const { return m_handle; }

private:

    std::reference_wrapper<Device> m_device;
    std::reference_wrapper<Instance> m_instance;

    vk::UniqueSurfaceKHR m_surface_handle;
    vk::UniqueSwapchainKHR m_handle;

    uint32 m_buffer_count;
    //std::vector<ImageView> m_image_views;
};

}