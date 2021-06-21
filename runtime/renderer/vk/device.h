// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Device final {
friend class Swapchain;
public:

    Device(const Adapter& adapter) : m_adapter(adapter) {

        uint32_t family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(m_adapter.m_phys_device, &family_count, nullptr);

        std::vector<VkQueueFamilyProperties> families(family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(m_adapter.m_phys_device, &family_count, families.data());

        std::vector<std::reference_wrapper<VkQueueFamilyProperties>> families_ref(families.begin(), families.end());
        std::sort(families_ref.begin(), families_ref.end(), [](const auto& a, const auto& b) {
            return std::bitset<8>(a.get().queueFlags).count() < std::bitset<8>(b.get().queueFlags).count();
        });

        std::optional<std::tuple<uint32, uint32>> graphics_index, transfer_index, compute_index;

        for(uint32 i = 0; i < families_ref.size(); i++) {
            for(uint32 j = 0; j < families_ref[i].get().queueCount; j++) {
                if(families_ref[i].get().queueFlags & VK_QUEUE_GRAPHICS_BIT && !graphics_index.has_value()) {
                    graphics_index = {&families_ref[i].get() - families.data(), j};
                } else if(families_ref[i].get().queueFlags & VK_QUEUE_TRANSFER_BIT && !transfer_index.has_value()) {
                    transfer_index = {&families_ref[i].get() - families.data(), j};
                } else if(families_ref[i].get().queueFlags & VK_QUEUE_COMPUTE_BIT && !compute_index.has_value()) {
                    compute_index = {&families_ref[i].get() - families.data(), j};
                }
            }
        }

        std::vector<VkDeviceQueueCreateInfo> queue_infos(families.size());
        std::vector<std::vector<float>> queue_priorities(families.size());

        for(uint32 i = 0; i < families.size(); i++) {
            queue_priorities[i].resize(families[i].queueCount);
            std::fill(queue_priorities[i].begin(), queue_priorities[i].end(), 1.0f);

            VkDeviceQueueCreateInfo queue_info = {};
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info.queueFamilyIndex = i;
            queue_info.queueCount = families[i].queueCount;
            queue_info.pQueuePriorities = queue_priorities[i].data();
            queue_infos[i] = queue_info;
        }


        const std::array<const char*, 1> device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkDeviceCreateInfo device_info = {};
        device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_info.pQueueCreateInfos = queue_infos.data();
        device_info.queueCreateInfoCount = static_cast<uint32>(queue_infos.size());
        device_info.ppEnabledExtensionNames = device_extensions.data();
        device_info.enabledExtensionCount = static_cast<uint32>(device_extensions.size());
        
        throw_if_failed(vkCreateDevice(m_adapter.m_phys_device, &device_info, nullptr, &m_device));

        if(!graphics_index.has_value()) {
            throw std::runtime_error("VkDevice Error (Your GPU not supported by application)");
        }

        vkGetDeviceQueue(m_device, std::get<0>(graphics_index.value()), std::get<1>(graphics_index.value()), &m_queues.graphics);
        if(!transfer_index.has_value()) {
            vkGetDeviceQueue(m_device, std::get<0>(graphics_index.value()), std::get<1>(graphics_index.value()), &m_queues.transfer);
        }
        if(!compute_index.has_value()) {
            vkGetDeviceQueue(m_device, std::get<0>(graphics_index.value()), std::get<1>(graphics_index.value()), &m_queues.compute);
        }
        vkGetDeviceQueue(m_device, std::get<0>(transfer_index.value()), std::get<1>(transfer_index.value()), &m_queues.transfer);
        vkGetDeviceQueue(m_device, std::get<0>(compute_index.value()), std::get<1>(compute_index.value()), &m_queues.compute);
    }

    ~Device() {
        vkDestroyDevice(m_device, nullptr);
    }

private:

    const Adapter& m_adapter;
    VkDevice m_device;

    struct {
        VkQueue graphics;
        VkQueue transfer;
        VkQueue compute;
    } m_queues;
};

}