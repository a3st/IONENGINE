// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Device {
friend class Swapchain;
friend class Shader;
public:

    Device() {
        
        initialize_instance();
        initialize_adapter();
        initialize_device();
    }

    Device(const Device&) = delete;
    Device(Device&& rhs) noexcept = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&& rhs) noexcept = delete;

    const AdapterConfig get_adapter_config() const { return m_adapter_config; }

private:

    vk::UniqueInstance m_instance;
    vk::PhysicalDevice m_adapter;
    vk::UniqueDevice m_device;

    AdapterConfig m_adapter_config;

    struct {
        int32 family_graphics;
        vk::Queue graphics;

        int32 family_compute;
        vk::Queue compute;

        int32 family_transfer;
        vk::Queue transfer;
    } m_queues;

    void initialize_instance() {

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
            .setPApplicationName("gfxcpp")
            .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
            .setPEngineName("gfxcpp")
            .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
            .setApiVersion(VK_API_VERSION_1_0);

        vk::InstanceCreateInfo instance_info{};

        instance_info
            .setPApplicationInfo(&app_info)
            .setPpEnabledExtensionNames(instance_extensions.data())
            .setEnabledExtensionCount(static_cast<uint32>(instance_extensions.size()))
            .setPpEnabledLayerNames(instance_layers.data())
            .setEnabledLayerCount(static_cast<uint32>(instance_layers.size()));

        m_instance = vk::createInstanceUnique(instance_info);
    }

    void initialize_adapter() {

        auto devices = m_instance->enumeratePhysicalDevices();
        for(const auto& device : devices) {

            vk::PhysicalDeviceProperties device_props{};
            device.getProperties(&device_props);

            if(device_props.deviceType != vk::PhysicalDeviceType::eVirtualGpu) {
                vk::PhysicalDeviceMemoryProperties device_memory_props{};
                device.getMemoryProperties(&device_memory_props);

                for(uint32 i = 0; i < device_memory_props.memoryHeapCount; ++i) {
                    if(device_memory_props.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal) {
                        m_adapter_config.dedicated_memory += device_memory_props.memoryHeaps[i].size;
                        break;
                    }
                }

                m_adapter_config.vendor_id = device_props.vendorID;
                m_adapter_config.device_id = device_props.deviceID;
                m_adapter_config.device_name = std::string(device_props.deviceName.data(), device_props.deviceName.size());
                m_adapter = device;
                break;
            }
        }
    }

    void initialize_device() {

        auto family_props = m_adapter.getQueueFamilyProperties();
        int32 graphics_family_idx = -1, compute_family_idx = -1, transfer_family_idx = -1;

        for(uint32 i = 0; i < family_props.size(); ++i) {
            
            // if supports graphics queue
            if(family_props[i].queueFlags & vk::QueueFlagBits::eGraphics) {
                graphics_family_idx = i;
            }

            // if supports compute queue
            if(family_props[i].queueFlags & vk::QueueFlagBits::eCompute) {
                compute_family_idx = i;
            }

            // if supports transfer queue
            if(family_props[i].queueFlags & vk::QueueFlagBits::eTransfer) {
                transfer_family_idx = i;
            }
        }

        if(graphics_family_idx == -1 || compute_family_idx == -1 || transfer_family_idx == -1) {
            throw std::runtime_error("Your GPU not supported by application");
        }

        m_queues.family_graphics = graphics_family_idx;
        m_queues.family_compute = compute_family_idx;
        m_queues.family_transfer = transfer_family_idx;

        std::vector<vk::DeviceQueueCreateInfo> queue_infos;
        std::vector<std::vector<float>> queue_priorities;
        queue_infos.resize(family_props.size());
        queue_priorities.resize(family_props.size());

        for(uint32 i = 0; i < queue_infos.size(); ++i) {
            queue_priorities[i].resize(family_props[i].queueCount);
            std::fill(queue_priorities[i].begin(), queue_priorities[i].end(), 1.0f);

            queue_infos[i]
                .setQueueCount(family_props[i].queueCount)
                .setPQueuePriorities(queue_priorities[i].data())
                .setQueueFamilyIndex(i);
        }

        std::vector<const char*> device_extensions;
        device_extensions.emplace_back("VK_KHR_swapchain");

        vk::DeviceCreateInfo device_info{};

        device_info
            .setPQueueCreateInfos(queue_infos.data())
            .setQueueCreateInfoCount(static_cast<uint32>(queue_infos.size()))
            .setPpEnabledExtensionNames(device_extensions.data())
            .setEnabledExtensionCount(static_cast<uint32>(device_extensions.size()));

        m_device = m_adapter.createDeviceUnique(device_info);

        m_queues.graphics = m_device->getQueue(m_queues.family_graphics, 0);
        m_queues.compute = m_device->getQueue(m_queues.family_compute, 0);
        m_queues.transfer = m_device->getQueue(m_queues.family_transfer, 0);
    }
};

}