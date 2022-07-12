// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/impl/device_vk.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

core::Expected<std::unique_ptr<Device>, std::string> Device_VK::create(
    platform::Window& window, uint16_t const sample_count) noexcept {
    
    auto device = std::make_unique<Device_VK>();

    vk::ApplicationInfo application_info;
    application_info
        .setPApplicationName("APP")
        .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
        .setPEngineName("IONENGINE")
        .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
        .setApiVersion(VK_API_VERSION_1_2);

    vk::InstanceCreateInfo instance_info;
    instance_info
        .setPApplicationInfo(&application_info);
    
    auto result = vk::createInstanceUnique(instance_info);
    
    if(result.result != vk::Result::eSuccess) {
        return core::make_expected<std::unique_ptr<Device>, std::string>("Can't initialize");
    }

    vk::UniqueInstance instance = std::move(result.value);

    return core::make_expected<std::unique_ptr<Device>, std::string>(std::move(device));
}

core::Expected<std::unique_ptr<Device>, std::string> Device::create(
    platform::Window& window, uint16_t const sample_count) noexcept {
    return Device_VK::create(window, sample_count);
}