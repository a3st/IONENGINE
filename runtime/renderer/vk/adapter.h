// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Adapter final {
public:

    Adapter(Instance& instance) : m_instance(instance), m_id(0), m_name("Invalid Device"), m_memory(0) {

        auto devices = instance.get_handle()->enumeratePhysicalDevices();

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

    Adapter(const Adapter&) = delete;

    Adapter(Adapter&& rhs) noexcept : m_instance(rhs.m_instance) {

        m_handle = rhs.m_handle;

        std::swap(m_id, rhs.m_id);
        std::swap(m_name, rhs.m_name);
        std::swap(m_memory, rhs.m_memory);
    }

    Adapter& operator=(const Adapter&) = delete;

    Adapter& operator=(Adapter&& rhs) noexcept {

        std::swap(m_instance, rhs.m_instance);

        m_handle = rhs.m_handle;

        std::swap(m_id, rhs.m_id);
        std::swap(m_name, rhs.m_name);
        std::swap(m_memory, rhs.m_memory);
        return *this;
    }

    const std::string& get_name() const { return m_name; }
    uint64 get_id() const { return m_id; }
    usize get_memory() const { return m_memory; }
    const vk::PhysicalDevice& get_handle() const { return m_handle; }

private:

    std::reference_wrapper<Instance> m_instance;

    vk::PhysicalDevice m_handle;

    uint64 m_id;
    std::string m_name;
    usize m_memory;
};

}