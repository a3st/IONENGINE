// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Adapter final {
public:

    Adapter(const Instance& instance) : 
        m_instance(instance), 
        m_id(0), m_name("Unknown device"), m_memory(0) {
        
        uint32_t device_count = 0;
        throw_if_failed(vkEnumeratePhysicalDevices(m_instance.get_handle(), &device_count, nullptr));

        std::vector<VkPhysicalDevice> devices(device_count);
        throw_if_failed(vkEnumeratePhysicalDevices(m_instance.get_handle(), &device_count, devices.data()));

        for(const auto& device : devices) {

            VkPhysicalDeviceProperties device_props = {};
            vkGetPhysicalDeviceProperties(device, &device_props);

            if(device_props.deviceType != VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
                VkPhysicalDeviceMemoryProperties device_memory_props = {};
                vkGetPhysicalDeviceMemoryProperties(device, &device_memory_props);

                for(uint32 i = 0; i < device_memory_props.memoryHeapCount; ++i) {
                    if(device_memory_props.memoryHeaps[i].flags && VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                        m_memory += device_memory_props.memoryHeaps[i].size;
                        break;
                    }
                }

                m_id = device_props.deviceID;
                m_name = device_props.deviceName;
                m_handle = device;
                break;
            }
        }
    }

    const std::string& get_name() const { return m_name; }
    uint64 get_id() const { return m_id; }
    usize get_memory() const { return m_memory; }
    const VkPhysicalDevice& get_handle() const { return m_handle; }

private:

    const Instance& m_instance;

    VkPhysicalDevice m_handle;
    uint64 m_id;
    std::string m_name;
    usize m_memory;
};

}