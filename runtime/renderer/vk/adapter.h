// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Adapter final {
friend class Device;
public:

    Adapter(const Instance& instance) : 
        m_instance(instance), 
        m_id(0), m_name("Unknown device"), m_memory(0) {
        
        uint32_t device_count = 0;
        throw_if_failed(vkEnumeratePhysicalDevices(m_instance.m_handle, &device_count, nullptr));

        std::vector<VkPhysicalDevice> devices(device_count);
        throw_if_failed(vkEnumeratePhysicalDevices(m_instance.m_handle, &device_count, devices.data()));

        for(uint32 i = 0; i < device_count; ++i) {

            VkPhysicalDeviceProperties device_props = {};
            vkGetPhysicalDeviceProperties(devices[i], &device_props);

            if(device_props.deviceType != VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {
                VkPhysicalDeviceMemoryProperties device_memory_props = {};
                vkGetPhysicalDeviceMemoryProperties(devices[i], &device_memory_props);

                for(uint32 j = 0; j < device_memory_props.memoryHeapCount; ++j) {
                    if(device_memory_props.memoryHeaps[j].flags && VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                        m_memory += device_memory_props.memoryHeaps[j].size;
                        break;
                    }
                }

                m_id = device_props.deviceID;
                m_name = device_props.deviceName;
                m_handle = devices[i];
                break;
            }
        }
    }

    const std::string& get_name() const { return m_name; }
    uint64 get_id() const { return m_id; }
    usize get_memory() const { return m_memory; }

private:

    const Instance& m_instance;

    VkPhysicalDevice m_handle;
    uint64 m_id;
    std::string m_name;
    usize m_memory;
};

}