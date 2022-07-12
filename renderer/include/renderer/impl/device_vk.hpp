// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/device.hpp>

#if defined(_WIN64)
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vulkan/vulkan.hpp>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.hpp>

namespace ionengine::renderer {

class Device_VK final : public Device {
 public:
    static core::Expected<std::unique_ptr<Device>, std::string> create(
        platform::Window& window, uint16_t const sample_count) noexcept;

 private:
    vk::UniqueInstance instance;
    vk::PhysicalDevice adapter;
    vk::UniqueDevice device;
    vk::UniqueSwapchainKHR swapchain;

    vma::Allocator memory_allocator;

    std::array<vk::Queue, 3> queues;
    std::array<vk::UniqueSemaphore, 3> semaphores;
    std::array<uint64_t, 3> fence_values;

    friend class Texture_D3D12;
};

}  // namespace ionengine::renderer
