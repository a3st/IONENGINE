// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace ionengine::gfx {

class VKDevice : public Device {
public:

    VKDevice(const uint32 adapter_index, void* window, const uint32 width, const uint32 height, const uint32 buffer_count, const uint32 multisample_count) {

        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = m_vk_dynamic_loader.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");
        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        vk::ApplicationInfo app_info{};
        app_info.apiVersion = VK_API_VERSION_1_2;

        std::vector<const char*> required_extensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        };

        vk::InstanceCreateInfo instance_create_info{};
        instance_create_info.pApplicationInfo = &app_info;
        instance_create_info.ppEnabledExtensionNames = required_extensions.data();
        instance_create_info.enabledExtensionCount = static_cast<uint32>(required_extensions.size());
        //instance_create_info.ppEnabledLayerNames =  ;
        //instance_create_info.enabledLayerCount = ;

        m_vk_instance = vk::createInstanceUnique(instance_create_info);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vk_instance.get());
    }

    void wait(const CommandListType command_list_type, Fence* fence, const uint64 value) override {

    }

    void signal(const CommandListType command_list_type, Fence* fence, const uint64 value) override {

    }

    void execute_command_buffers(const CommandListType command_list_type, const std::vector<CommandList*>& command_buffers) override {

    }

    std::unique_ptr<Resource> create_resource(const ResourceType type, const ResourceDesc& resource_desc) override {
        return std::make_unique<VKResource>();
    }

    std::unique_ptr<Sampler> create_sampler(const SamplerDesc& sampler_desc) override {
        return std::make_unique<VKSampler>();
    }

    std::unique_ptr<Fence> create_fence(const uint64 initial_value) override {
        return std::make_unique<VKFence>(&m_vk_device.get(), initial_value);
    }

    std::unique_ptr<Memory> allocate_memory(const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) override {
        return std::make_unique<VKMemory>();
    }

    void present() override {
        
    }

    Resource* get_swapchain_resource(const uint32 resource_index) override { return nullptr; }

    uint32 get_swapchain_resource_index() const override { return 0; }

    const AdapterDesc& get_adapter_desc() const override { return m_adapter_desc; }

private:

    vk::DynamicLoader m_vk_dynamic_loader;
    vk::UniqueInstance m_vk_instance;
    vk::UniqueDevice m_vk_device;

    AdapterDesc m_adapter_desc;


};

std::unique_ptr<Device> create_unique_device(const uint32 adapter_index, void* window, const uint32 width, const uint32 height, const uint32 buffer_count, const uint32 multisample_count) {
    return std::make_unique<VKDevice>(adapter_index, window, width, height, buffer_count, multisample_count);
}

}