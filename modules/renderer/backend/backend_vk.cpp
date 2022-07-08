// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#define VULKAN_HPP_NO_EXCEPTIONS

#include <../precompiled.h>
#include <renderer/backend/backend.h>
#include <vulkan/vulkan.hpp>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.hpp>

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
    vk::PhysicalDevice adapter;
    vk::UniqueDevice device;
    vk::UniqueSwapchainKHR swapchain;

    vma::Allocator memory_allocator;

    std::array<vk::Queue, 3> queues;
    std::array<vk::UniqueSemaphore, 3> semaphores;
    std::array<uint64_t, 3> fence_values;

    HandlePool<Texture> textures;
    HandlePool<Buffer> buffers;
    HandlePool<Sampler> samplers;
    HandlePool<DescriptorLayout> descriptor_layouts;
    HandlePool<Pipeline> pipelines;
    HandlePool<Shader> shaders;
    HandlePool<RenderPass> render_passes;
    HandlePool<CommandList> command_lists;

    uint16_t swapchain_index;
    std::array<Handle<Texture>, 3> swapchain_textures;
};

void Device::impl_deleter::operator()(Impl* ptr) const { delete ptr; }

Device::Device(uint32_t const adapter_index, platform::Window& window, uint16_t const sample_count) {

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
        return;
    }

    vk::UniqueInstance instance = std::move(result.value);

    _impl = std::unique_ptr<Device::Impl, impl_deleter>(new Device::Impl { .instance = std::move(instance) });
}
