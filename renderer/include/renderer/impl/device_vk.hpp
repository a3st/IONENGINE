// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/device.hpp>
#include <renderer/impl/vk.hpp>

namespace ionengine::renderer {

///
/// @private
///
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

    friend class Texture_VK;
};

}  // namespace ionengine::renderer
