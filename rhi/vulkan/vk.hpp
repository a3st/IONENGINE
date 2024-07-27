// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#ifdef IONENGINE_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#elif IONENGINE_PLATFORM_X11
#define VK_USE_PLATFORM_XLIB_KHR
#endif
#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#undef Always
#undef None
#undef max
#undef min
#include <xxhash.h>

namespace ionengine::rhi
{
    enum class DescriptorAllocatorLimits : uint32_t
    {
        Uniform = 16 * 1024,
        Storage = 16 * 1024,
        SampledImage = 16 * 1024,
        Sampler = 128
    };

    VK_DEFINE_HANDLE(DescriptorAllocation);

    class DescriptorAllocator;

    struct DescriptorAllocation_T
    {
        VkDescriptorType descriptorType;
        VkDescriptorSet descriptorSet;
        uint32_t binding;
        uint32_t arrayElement;
    };

    class DescriptorAllocator final : public core::ref_counted_object
    {
      public:
        DescriptorAllocator(VkDevice device);

        ~DescriptorAllocator();

        auto allocate(VkDescriptorType const descriptorType, DescriptorAllocation* allocation) -> VkResult;

        auto deallocate(DescriptorAllocation allocation) -> void;

      private:
        std::mutex mutex;
        VkDevice device;
        VkDescriptorSetLayout descriptorSetLayout;

        struct Chunk
        {
            VkDescriptorSet descriptorSet;
            uint32_t binding;
            std::vector<uint8_t> free;
            uint32_t offset;
            uint32_t size;
            std::vector<DescriptorAllocation_T> allocations;
        };

        std::unordered_map<VkDescriptorType, Chunk> chunks;
    };

    class VKVertexInput
    {
      public:
        VKVertexInput(std::span<VertexDeclarationInfo const> const vertexDeclarations);

        auto getPipelineVertexInputState() const -> VkPipelineVertexInputStateCreateInfo;

      private:
        VkVertexInputBindingDescription inputBinding;
        std::vector<VkVertexInputAttributeDescription> inputAttributes;
    };

    struct VKShaderStage
    {
        std::string entryPoint;
        VkShaderModule shaderModule;
    };

    class VKShader : public Shader
    {
      public:
        VKShader(VkDevice device, ShaderCreateInfo const& createInfo);

        ~VKShader();

        auto getHash() const -> uint64_t override;

        auto getPipelineType() const -> PipelineType override;

        auto getStages() const -> std::unordered_map<VkShaderStageFlagBits, VKShaderStage> const&;

        auto getVertexInput() const -> std::optional<VKVertexInput>;

      private:
        VkDevice device;
        std::optional<VKVertexInput> vertexInput;
        std::unordered_map<VkShaderStageFlagBits, VKShaderStage> stages;
        uint64_t hash;
        PipelineType pipelineType;
    };

    class Pipeline : public core::ref_counted_object
    {
      public:
        Pipeline(VkDevice device, VkPipelineLayout pipelineLayout, VKShader* shader,
                 RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                 std::optional<DepthStencilStageInfo> const depthStencil,
                 std::span<VkFormat const> const renderTargetFormats, VkFormat const depthStencilFormat,
                 VkPipelineCache pipelineCache);

        ~Pipeline();

      private:
        VkDevice device;
        VkPipeline pipeline;
    };

    class PipelineCache final : public core::ref_counted_object
    {
      public:
        struct Entry
        {
            uint64_t shaderHash;
            RasterizerStageInfo rasterizer;
            BlendColorInfo blendColor;
            std::optional<DepthStencilStageInfo> depthStencil;
            std::array<VkFormat, 8> renderTargetFormats;
            VkFormat depthStencilFormat;

            auto operator==(Entry const& other) const -> bool
            {
                return std::make_tuple(shaderHash, rasterizer, blendColor,
                                       depthStencil.value_or(DepthStencilStageInfo::Default()), renderTargetFormats,
                                       depthStencilFormat) ==
                       std::make_tuple(other.shaderHash, other.rasterizer, other.blendColor,
                                       other.depthStencil.value_or(DepthStencilStageInfo::Default()),
                                       other.renderTargetFormats, other.depthStencilFormat);
            }
        };

        struct EntryHasher
        {
            auto operator()(const Entry& entry) const -> std::size_t
            {
                auto depthStencil = entry.depthStencil.value_or(DepthStencilStageInfo::Default());
                return entry.shaderHash ^ XXH64(&entry.rasterizer.fillMode, sizeof(uint32_t), 0) ^
                       XXH64(&entry.rasterizer.cullMode, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendDst, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendDstAlpha, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendEnable, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendOp, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendOpAlpha, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendSrc, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendSrcAlpha, sizeof(uint32_t), 0) ^
                       XXH64(&depthStencil.depthFunc, sizeof(uint32_t), 0) ^
                       XXH64(&depthStencil.depthWrite, sizeof(uint32_t), 0) ^
                       XXH64(&depthStencil.stencilWrite, sizeof(uint32_t), 0) ^
                       XXH64(entry.renderTargetFormats.data(), entry.renderTargetFormats.size(), 0) ^
                       XXH64(&entry.depthStencilFormat, sizeof(VkFormat), 0);
            }
        };

        PipelineCache(VkDevice device, RHICreateInfo const& createInfo);

        ~PipelineCache();

        auto get(VKShader* shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                 std::optional<DepthStencilStageInfo> const depthStencil,
                 std::array<VkFormat, 8> const& renderTargetFormats,
                 VkFormat const depthStencilFormat) -> core::ref_ptr<Pipeline>;

        auto reset() -> void;

      private:
        std::mutex mutex;
        VkDevice device;
        VkPipelineLayout pipelineLayout;
        std::unordered_map<Entry, core::ref_ptr<Pipeline>, EntryHasher> entries;
    };

    class VKBuffer final : public Buffer
    {
        VKBuffer(VkDevice device, VmaAllocator memoryAllocator, BufferCreateInfo const& createInfo);

        ~VKBuffer();

        inline static uint32_t const ConstantBufferSizeAlignment = 256;

        auto getSize() -> size_t override;

        auto getFlags() -> BufferUsageFlags override;

        auto mapMemory() -> uint8_t* override;

        auto unmapMemory() -> void override;

        auto getBuffer() const -> VkBuffer;

        auto getDescriptorOffset(BufferUsage const usage) const -> uint32_t override;

      private:
        VkBuffer buffer;
    };

    class VKTexture final : public Texture
    {
      public:
        VKTexture(VkDevice device, VmaAllocator memoryAllocator, TextureCreateInfo const& createInfo);

        /*!
            @brief Constructor for Swapchain Texture
        */
        VKTexture(VkDevice device, VkImage image, uint32_t const width, uint32_t const height);

        ~VKTexture();

        auto getWidth() const -> uint32_t override;

        auto getHeight() const -> uint32_t override;

        auto getDepth() const -> uint32_t override;

        auto getMipLevels() const -> uint32_t override;

        auto getFormat() const -> TextureFormat override;

        auto getFlags() const -> TextureUsageFlags override;

        auto getDescriptorOffset(TextureUsage const usage) const -> uint32_t override;

        auto getImage() const -> VkImage;

        auto getImageView() const -> VkImageView;

        auto getInitialLayout() const -> VkImageLayout;

      private:
        VkDevice device;
        VmaAllocator memoryAllocator;
        VkImage image;
        VmaAllocation memoryAllocation;
        VkImageView imageView;
        VkImageLayout initialLayout;
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
        uint32_t queueFamilyIndex;
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

        auto createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler> override;

        auto createGraphicsContext() -> core::ref_ptr<GraphicsContext> override;

        auto createCopyContext() -> core::ref_ptr<CopyContext> override;

        auto requestBackBuffer() -> core::ref_ptr<Texture> override;

        auto presentBackBuffer() -> void override;

        auto resizeBackBuffers(uint32_t const width, uint32_t const height) -> void override;

        auto getBackendName() const -> std::string_view override;

      private:
        VkInstance instance;
#ifndef NDEBUG
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

        core::ref_ptr<DescriptorAllocator> descriptorAllocator;

        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        VkSemaphore acquireSemaphore;
        VkSemaphore presentSemaphore;
        uint32_t imageIndex;

        std::vector<core::ref_ptr<VKTexture>> backBuffers;

        auto createSwapchain(uint32_t const width, uint32_t const height) -> void;
    };
} // namespace ionengine::rhi