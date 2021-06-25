// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Instance final {
friend class Adapter;
friend class Device;
friend class Swapchain;
public:

    Instance() {
        std::vector<const char*> instance_layers;
        std::vector<const char*> instance_extensions;

        instance_extensions.emplace_back("VK_KHR_surface");

#ifdef NDEBUG
        instance_layers.emplace_back("VK_LAYER_LUNARG_standard_validation");
        instance_extensions.emplace_back("VK_EXT_debug_utils");
#endif

#ifdef VK_USE_PLATFORM_WIN32_KHR
        instance_extensions.emplace_back("VK_KHR_win32_surface");
#endif

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
        instance_info.ppEnabledExtensionNames = instance_extensions.data();
        instance_info.enabledExtensionCount = static_cast<uint32>(instance_extensions.size());
        instance_info.ppEnabledLayerNames = instance_layers.data();
        instance_info.enabledLayerCount = static_cast<uint32>(instance_layers.size());

        throw_if_failed(vkCreateInstance(&instance_info, nullptr, &m_handle));
    }

    ~Instance() {
        vkDestroyInstance(m_handle, nullptr);
    }

private:

    VkInstance m_handle;
};

}