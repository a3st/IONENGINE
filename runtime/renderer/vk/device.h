// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Device final {
public:

    Device(Adapter& adapter) : m_adapter(adapter), m_family_ids { -1, -1, -1 } {

        auto family_props = adapter.get_handle().getQueueFamilyProperties();
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

        m_family_ids.graphics = graphics_family_idx;
        m_family_ids.compute = compute_family_idx;
        m_family_ids.transfer = transfer_family_idx;

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

        m_handle = m_adapter.get().get_handle().createDeviceUnique(device_info);

        m_queue_handles.graphics = m_handle->getQueue(m_family_ids.graphics, 0);
        m_queue_handles.compute = m_handle->getQueue(m_family_ids.compute, 0);
        m_queue_handles.transfer = m_handle->getQueue(m_family_ids.transfer, 0);
    }

    Device(const Device&) = delete;

    Device(Device&& rhs) noexcept : m_adapter(rhs.m_adapter) {

        m_handle.swap(rhs.m_handle);
        
        std::swap(m_queue_handles, rhs.m_queue_handles);
        std::swap(m_family_ids, rhs.m_family_ids);
    }

    Device& operator=(const Device&) = delete;

    Device& operator=(Device&& rhs) {

        std::swap(m_adapter, rhs.m_adapter);

        m_handle.swap(rhs.m_handle);
        
        std::swap(m_queue_handles, rhs.m_queue_handles);
        std::swap(m_family_ids, rhs.m_family_ids);
        return *this;
    }

    const vk::UniqueDevice& get_handle() const { return m_handle; }
    const vk::PhysicalDevice& get_adapter_handle() const { return m_adapter.get().get_handle(); }

    int32 get_graphics_family_index() const { return m_family_ids.graphics; };
    int32 get_compute_family_index() const { return m_family_ids.compute; };
    int32 get_transfer_family_index() const { return m_family_ids.transfer; };

private:

    std::reference_wrapper<Adapter> m_adapter;

    vk::UniqueDevice m_handle;

    struct {
        vk::Queue graphics;
        vk::Queue compute;
        vk::Queue transfer;
    } m_queue_handles;

    struct {
        int32 graphics;
        int32 compute;
        int32 transfer;
    } m_family_ids;
};

}