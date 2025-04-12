// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include <xxhash.h>
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

    class DescriptorAllocator
    {
      public:
        DescriptorAllocator(VkDevice device);

        ~DescriptorAllocator();

        auto allocate(VkDescriptorType const descriptorType, DescriptorAllocation* allocation) -> VkResult;

        auto deallocate(DescriptorAllocation allocation) -> void;

        auto getDescriptorSet(VkDescriptorType const descriptorType) -> VkDescriptorSet;

        auto getDescriptorSetLayout(VkDescriptorType const descriptorType) -> VkDescriptorSetLayout;

      private:
        std::mutex mutex;
        VkDevice device;
        VkDescriptorPool descriptorPool;

        struct Chunk
        {
            VkDescriptorSetLayout descriptorSetLayout;
            VkDescriptorSet descriptorSet;
            uint32_t binding;
            std::vector<uint8_t> free;
            uint32_t offset;
            uint32_t size;
            std::vector<DescriptorAllocation_T> allocations;
        };

        std::unordered_map<VkDescriptorType, Chunk> chunks;

        auto createChunk(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorType const descriptorType,
                         uint32_t const descriptorCount) -> void;
    };

    class VKVertexInput
    {
      public:
        VKVertexInput(std::span<VertexDeclarationInfo const> const vertexDeclarations);

        auto getPipelineVertexInputState() const -> VkPipelineVertexInputStateCreateInfo const&;

      private:
        VkVertexInputBindingDescription inputBinding;
        std::vector<VkVertexInputAttributeDescription> inputAttributes;
        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    };

    struct VKShaderStage
    {
        std::string entryPoint;
        VkShaderModule shaderModule;
    };

    class VKShader final : public Shader
    {
      public:
        VKShader(VkDevice device, ShaderCreateInfo const& createInfo);

        ~VKShader();

        auto getHash() const -> uint64_t;

        auto getShaderType() const -> ShaderType override;

        auto getStages() const -> std::unordered_map<VkShaderStageFlagBits, VKShaderStage> const&;

        auto getVertexInput() const -> std::optional<VKVertexInput> const&;

      private:
        VkDevice device;
        std::optional<VKVertexInput> vertexInput;
        std::unordered_map<VkShaderStageFlagBits, VKShaderStage> stages;
        uint64_t hash;
        ShaderType shaderType;
    };

    class Pipeline : public core::ref_counted_object
    {
      public:
        Pipeline(VkDevice device, VkPipelineLayout pipelineLayout, VKShader* shader,
                 RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                 std::optional<DepthStencilStageInfo> const depthStencil,
                 std::array<VkFormat, 8> const& renderTargetFormats, VkFormat const depthStencilFormat,
                 VkPipelineCache pipelineCache);

        ~Pipeline();

        auto getPipelineType() const -> VkPipelineBindPoint;

        auto getPipeline() -> VkPipeline;

      private:
        VkDevice device;
        VkPipeline pipeline;
        VkPipelineBindPoint pipelineType;
    };

    class PipelineCache
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
            auto operator()(const Entry& other) const -> std::size_t
            {
                auto depthStencil = other.depthStencil.value_or(DepthStencilStageInfo::Default());
                return other.shaderHash ^ ::XXH64(&other.rasterizer.fillMode, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.rasterizer.cullMode, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendDst, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendDstAlpha, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendEnable, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendOp, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendOpAlpha, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendSrc, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendSrcAlpha, sizeof(uint32_t), 0) ^
                       ::XXH64(&depthStencil.depthFunc, sizeof(uint32_t), 0) ^
                       ::XXH64(&depthStencil.depthWrite, sizeof(uint32_t), 0) ^
                       ::XXH64(&depthStencil.stencilWrite, sizeof(uint32_t), 0) ^
                       ::XXH64(other.renderTargetFormats.data(), other.renderTargetFormats.size(), 0) ^
                       ::XXH64(&other.depthStencilFormat, sizeof(VkFormat), 0);
            }
        };

        PipelineCache(VkDevice device, DescriptorAllocator* descriptorAllocator, RHICreateInfo const& createInfo);

        ~PipelineCache();

        auto get(VKShader* shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                 std::optional<DepthStencilStageInfo> const depthStencil,
                 std::array<VkFormat, 8> const& renderTargetFormats, VkFormat const depthStencilFormat)
            -> core::ref_ptr<Pipeline>;

        auto reset() -> void;

        auto getPipelineLayout() -> VkPipelineLayout;

      private:
        std::mutex mutex;
        VkDevice device;
        VkPipelineLayout pipelineLayout;
        std::unordered_map<Entry, core::ref_ptr<Pipeline>, EntryHasher> entries;
    };

    class VKBuffer final : public Buffer
    {
      public:
        VKBuffer(VkDevice device, VmaAllocator memoryAllocator, BufferCreateInfo const& createInfo);

        ~VKBuffer();

        auto getSize() const -> size_t override;

        auto getFlags() const -> BufferUsageFlags override;

        auto mapMemory() -> uint8_t*;

        auto unmapMemory() -> void;

        auto getBuffer() -> VkBuffer;

        auto getDescriptorOffset(BufferUsage const usage) const -> uint32_t override;

      private:
        VkDevice device;
        VmaAllocator memoryAllocator;
        VkBuffer buffer;
        VmaAllocation memoryAllocation;
        size_t size;
        BufferUsageFlags flags;
    };

    class VKTexture final : public Texture
    {
      public:
        VKTexture(VkDevice device, VmaAllocator memoryAllocator, DescriptorAllocator* descriptorAllocator,
                  TextureCreateInfo const& createInfo);

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

        auto getImage() -> VkImage;

        auto getImageView() -> VkImageView;

        auto getInitialLayout() const -> VkImageLayout;

      private:
        VkDevice device;
        VmaAllocator memoryAllocator;
        DescriptorAllocator* descriptorAllocator;
        VkImage image;
        VmaAllocation memoryAllocation;
        std::unordered_map<TextureUsage, DescriptorAllocation> descriptorAllocations;
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

    class VKSampler final : public Sampler
    {
      public:
        VKSampler(VkDevice device, SamplerCreateInfo const& createInfo);

        ~VKSampler();

        auto getDescriptorOffset() const -> uint32_t override;

      private:
        VkDevice device;
        VkSampler sampler;
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

    struct DeviceQueueData
    {
        VkQueue queue;
        VkSemaphore semaphore;
        uint32_t familyIndex;
        uint64_t fenceValue;
    };

    class VKGraphicsContext final : public GraphicsContext
    {
      public:
        VKGraphicsContext(VkDevice device, PipelineCache* pipelineCache, DescriptorAllocator* descriptorAllocator,
                          DeviceQueueData& deviceQueue);

        ~VKGraphicsContext();

        auto setGraphicsPipelineOptions(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                        BlendColorInfo const& blendColor,
                                        std::optional<DepthStencilStageInfo> const depthStencil) -> void override;

        auto bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void override;

        auto beginRenderPass(std::span<RenderPassColorInfo const> const colors,
                             std::optional<RenderPassDepthStencilInfo> depthStencil) -> void override;

        auto endRenderPass() -> void override;

        auto bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void override;

        auto bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                             IndexFormat const format) -> void override;

        auto drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void override;

        auto draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void override;

        auto setViewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
            -> void override;

        auto setScissor(int32_t const left, int32_t const top, int32_t const right, int32_t const bottom)
            -> void override;

        auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto execute() -> Future<void> override;

      private:
        VkDevice device;
        PipelineCache* pipelineCache;
        DescriptorAllocator* descriptorAllocator;
        DeviceQueueData* deviceQueue;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        VkRect2D renderArea;
        bool isCommandListOpened;
        std::array<VkFormat, 8> renderTargetFormats;
        VkFormat depthStencilFormat;
        std::vector<uint8_t> bindingData;

        auto tryAllocateCommandBuffer() -> void;
    };

    class VKCopyContext final : public CopyContext
    {
      public:
        VKCopyContext(VkDevice device, VmaAllocator memoryAllocator, DeviceQueueData& deviceQueue,
                      RHICreateInfo const& rhiCreateInfo);

        ~VKCopyContext();

        auto updateBuffer(core::ref_ptr<Buffer> dest, uint64_t const offset, std::span<uint8_t const> const dataBytes)
            -> Future<Buffer> override;

        auto updateTexture(core::ref_ptr<Texture> dest, uint32_t const resourceIndex,
                           std::span<uint8_t const> const dataBytes) -> Future<Texture> override;

        auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto execute() -> Future<void> override;

      private:
        VkDevice device;
        DeviceQueueData* deviceQueue;
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        bool isCommandListOpened;

        struct StagingBufferData
        {
            core::ref_ptr<VKBuffer> buffer;
            uint64_t offset;
        };

        StagingBufferData readStagingBuffer;
        StagingBufferData writeStagingBuffer;

        /*auto getSurfaceData(DXGI_FORMAT const format, uint32_t const width, uint32_t const height, size_t& rowBytes,
                            uint32_t& rowCount) -> void;*/

        auto tryAllocateCommandBuffer() -> void;
    };

    class VKSwapchain final : public Swapchain
    {
      public:
        VKSwapchain(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device, DeviceQueueData& deviceQueue,
                    SwapchainCreateInfo const& createInfo);

        ~VKSwapchain();

        auto getBackBuffer() -> core::weak_ptr<Texture> override;

        auto presentBackBuffer() -> void override;

        auto resizeBackBuffers(uint32_t const width, uint32_t const height) -> void override;

      private:
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSurfaceKHR surface;
        VkSwapchainKHR swapchain;
        DeviceQueueData* deviceQueue;
        VkSemaphore acquireSemaphore;
        VkSemaphore presentSemaphore;
        uint32_t imageIndex;
        std::vector<core::ref_ptr<Texture>> backBuffers;

        auto createSwapchainBuffers(uint32_t const width, uint32_t const height) -> void;
    };

    class VKRHI final : public RHI
    {
      public:
        VKRHI(RHICreateInfo const& createInfo);

        ~VKRHI();

        auto createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader> override;

        auto createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture> override;

        auto createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer> override;

        auto createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler> override;

        auto getSwapchain() -> Swapchain* override;

        auto getGraphicsContext() -> GraphicsContext* override;

        auto getCopyContext() -> CopyContext* override;

        auto getName() const -> std::string const& override;

      private:
        VkInstance instance;
#ifndef NDEBUG
        VkDebugUtilsMessengerEXT debugUtilsMessenger;

        PFN_vkCreateDebugUtilsMessengerEXT createDebugUtilsMessengerEXT;
        PFN_vkDestroyDebugUtilsMessengerEXT destroyDebugUtilsMessengerEXT;
#endif
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VmaAllocator memoryAllocator;

        DeviceQueueData graphicsQueue;
        DeviceQueueData transferQueue;
        DeviceQueueData computeQueue;

        std::unique_ptr<DescriptorAllocator> descriptorAllocator;
        std::unique_ptr<PipelineCache> pipelineCache;

        std::unique_ptr<VKSwapchain> swapchain;
        std::unique_ptr<VKGraphicsContext> graphicsContext;
        std::unique_ptr<VKCopyContext> copyContext;

        std::string const rhiName{"Vulkan"};

        auto createDeviceQueue(uint32_t const queueFamily, DeviceQueueData& deviceQueue) -> void;
    };
} // namespace ionengine::rhi