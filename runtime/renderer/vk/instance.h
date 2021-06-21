// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/types.h"

namespace ionengine::renderer {

class Instance final {
friend class Adapter;
friend class Device;
friend class Swapchain;
public:

    Instance() {

        std::array<const char*, 1> vk_layers = {
            "VK_LAYER_LUNARG_standard_validation"
        };

        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "IONENGINE";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "IONENGINE";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instance_info = {};
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_info.pApplicationInfo = &app_info;
        instance_info.ppEnabledExtensionNames = platform::vk_extensions.data();
        instance_info.enabledExtensionCount = static_cast<uint32>(platform::vk_extensions.size());
        instance_info.ppEnabledLayerNames = vk_layers.data();
        instance_info.enabledLayerCount = static_cast<uint32>(vk_layers.size());

        throw_if_failed(vkCreateInstance(&instance_info, nullptr, &m_instance));
    }

    ~Instance() {
        vkDestroyInstance(m_instance, nullptr);
    }

private:

    VkInstance m_instance;
};

}