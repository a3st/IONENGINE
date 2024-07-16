// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#define VK_USE_PLATFORM_XLIB_KHR
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace ionengine::rhi
{
    class DescriptorAllocator : public core::ref_counted_object
    {
      public:
        DescriptorAllocator();

      private:
    };

    class PipelineCache : public core::ref_counted_object
    {
    };

    class VKTexture final : public Texture
    {
      public:
        VKTexture(VkDevice device, VmaAllocator memoryAllocator, TextureCreateInfo const& createInfo);

        VKTexture(VkDevice device, VkImage image, TextureCreateInfo const& createInfo);

        ~VKTexture();

        auto getWidth() const -> uint32_t override;

        auto getHeight() const -> uint32_t override;

        auto getDepth() const -> uint32_t override;

        auto getMipLevels() const -> uint32_t override;

        auto getFormat() const -> TextureFormat override;

        auto getFlags() const -> TextureUsageFlags override;

        auto getDescriptorOffset(TextureUsage const usage) const -> uint32_t override;

        auto getImageView() const -> VkImageView;

      private:
        VkDevice device;
        VmaAllocator memoryAllocator;
        VkImage image;
        VmaAllocation memoryAllocation;
        VkImageView imageView;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        TextureFormat format;
        TextureDimension dimension;
        TextureUsageFlags flags;
    };

    class VKFutureImpl final : public FutureImpl
    {
      public:
        VKFutureImpl(VkDevice device, VkQueue queue, VkSemaphore semaphore, uint64_t const fenceValue);

        auto getResult() const -> bool override;

        auto wait() -> void override;

      private:
        VkDevice device;
        VkQueue queue;
        VkSemaphore semaphore;
        uint64_t fenceValue;
    };

    class VKQuery final : public Query
    {
    };

    class VKGraphicsContext final : public GraphicsContext
    {
      public:
        VKGraphicsContext(VkDevice device, VkQueue queue, uint32_t queueFamilyIndex, VkSemaphore semaphore,
                          uint64_t& fenceValue);

        ~VKGraphicsContext();

        auto reset() -> void override;

        auto execute() -> Future<Query> override;

        auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before,
                     ResourceState const after) -> void override;

        auto barrier(core::ref_ptr<Texture> dest, ResourceState const before,
                     ResourceState const after) -> void override;

        auto setGraphicsPipelineOptions(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                        BlendColorInfo const& blendColor,
                                        std::optional<DepthStencilStageInfo> const depthStencil) -> void override;

        auto bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void override;

        auto beginRenderPass(std::span<RenderPassColorInfo> const colors,
                             std::optional<RenderPassDepthStencilInfo> depthStencil) -> void override;

        auto endRenderPass() -> void override;

        auto bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void override;

        auto bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                             IndexFormat const format) -> void override;

        auto drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void override;

        auto draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void override;

        auto setViewport(int32_t const x, int32_t const y, uint32_t const width,
                         uint32_t const height) -> void override;

        auto setScissor(int32_t const left, int32_t const top, int32_t const right,
                        int32_t const bottom) -> void override;

      private:
        VkDevice device;
        VkQueue queue;
        VkSemaphore semaphore;
        uint64_t* fenceValue;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        VkRect2D renderArea;
    };

    class VKDevice final : public Device
    {
      public:
        VKDevice(RHICreateInfo const& createInfo);

        ~VKDevice();

        auto createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader> override;

        auto createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture> override;

        auto createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer> override;

        auto createGraphicsContext() -> core::ref_ptr<GraphicsContext> override;

        auto createCopyContext() -> core::ref_ptr<CopyContext> override;

        auto requestBackBuffer() -> core::ref_ptr<Texture> override;

        auto presentBackBuffer() -> void override;

        auto getBackendType() const -> std::string_view override;

      private:
        VkInstance instance;
#ifdef _DEBUG
        VkDebugUtilsMessengerEXT debugUtilsMessenger;

        PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT;
        PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT;
#endif
        VkPhysicalDevice physicalDevice;
        VkDevice device;

        struct QueueInfo
        {
            VkQueue queue;
            VkSemaphore semaphore;
            uint32_t familyIndex;
            uint64_t fenceValue;
        };
        QueueInfo graphicsQueue;
        QueueInfo transferQueue;
        QueueInfo computeQueue;

        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        VkSemaphore swapchainSemaphore;
        uint32_t nextImageIndex;

        std::vector<core::ref_ptr<VKTexture>> backBuffers;
    };
} // namespace ionengine::rhi