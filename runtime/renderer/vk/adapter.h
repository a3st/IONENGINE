// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Adapter final {
friend class Device;
public:

    Adapter(const Instance& instance) : m_instance(instance) {
        
        uint32_t device_count = 0;
        throw_if_failed(vkEnumeratePhysicalDevices(m_instance.m_instance, &device_count, nullptr));

        std::vector<VkPhysicalDevice> devices(device_count);
        throw_if_failed(vkEnumeratePhysicalDevices(m_instance.m_instance, &device_count, devices.data()));

        for(uint32 i = 0; i < device_count; i++) {
            VkPhysicalDeviceProperties device_props = {};
            vkGetPhysicalDeviceProperties(devices[i], &device_props);
            if(device_props.deviceType != VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU) {

                VkPhysicalDeviceMemoryProperties memory_props = {};
                vkGetPhysicalDeviceMemoryProperties(devices[i], &memory_props);

                for(uint32 j = 0; j < memory_props.memoryTypeCount; j++) {
                    if(memory_props.memoryTypes[j].propertyFlags && VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                        m_device_memory = memory_props.memoryHeaps[memory_props.memoryTypes[j].heapIndex].size;
                        break;
                    }
                }

                m_device_id = device_props.deviceID;
                m_device_name = device_props.deviceName;
                
                m_phys_device = devices[i];
                break;
            }
        }
    }

    const std::string& get_device_name() const {
        return m_device_name;
    }

    uint64 get_device_id() const { 
        return m_device_id; 
    }

    usize get_device_memory() const {
        return m_device_memory;
    }

private:

    const Instance& m_instance;
    VkPhysicalDevice m_phys_device;

    uint64 m_device_id;
    std::string m_device_name;
    usize m_device_memory;
};

}