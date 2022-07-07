// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <../precompiled.h>
#include <renderer/backend/backend.h>
#include <vulkan/vulkan.hpp>

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::backend;

namespace ionengine::renderer::backend {

///
/// @private
///
struct Texture {};

///
/// @private
///
struct Buffer {};

///
/// @private
///
struct Sampler {};

///
/// @private
///
struct DescriptorLayout {};

///
/// @private
///
struct Pipeline {};

///
/// @private
///
struct Shader {};

///
/// @private
///
struct RenderPass {};

///
/// @private
///
struct CommandList {};

}  // namespace ionengine::renderer::backend

///
/// @private
///
struct Device::Impl {
    
    vk::UniqueInstance instance;

    Impl();
};

void Device::impl_deleter::operator()(Impl* ptr) const { delete ptr; }

Device::Impl::Impl() {

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
    
    instance = vk::createInstanceUnique(instance_info);
}