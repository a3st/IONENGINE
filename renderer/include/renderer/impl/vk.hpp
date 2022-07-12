// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#if defined(_WIN64)
    #define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__)
    #define VK_USE_PLATFORM_XLIB_KHR
#endif
#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.hpp>

namespace ionengine::renderer {

inline std::string to_string(vk::Result const result) {
    switch(result) {
        case vk::Result::eErrorDeviceLost: return "VK_ERROR_DEVICE_LOST";
        default: return "VK_ERROR_UNKNOWN";
    }
}

}
