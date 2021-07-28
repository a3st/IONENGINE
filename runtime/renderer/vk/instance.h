// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Instance final {
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

        vk::ApplicationInfo app_info{};
        
        app_info
            .setPApplicationName("IONENGINE")
            .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
            .setPEngineName("IONENGINE")
            .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
            .setApiVersion(VK_API_VERSION_1_0);

        vk::InstanceCreateInfo instance_info{};

        instance_info
            .setPApplicationInfo(&app_info)
            .setPpEnabledExtensionNames(instance_extensions.data())
            .setEnabledExtensionCount(static_cast<uint32>(instance_extensions.size()))
            .setPpEnabledLayerNames(instance_layers.data())
            .setEnabledLayerCount(static_cast<uint32>(instance_layers.size()));

        m_handle = vk::createInstanceUnique(instance_info);
    }

    Instance(const Instance&) = delete;

    Instance(Instance&& rhs) noexcept {
        m_handle.swap(rhs.m_handle);
    }

    Instance& operator=(const Instance&) = delete;
    
    Instance& operator=(Instance&& rhs) {
        m_handle.swap(rhs.m_handle);
        return *this;
    }

    const vk::UniqueInstance& get_handle() const { return m_handle; }

private:

    vk::UniqueInstance m_handle;
};

}