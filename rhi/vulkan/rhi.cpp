// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#define VMA_IMPLEMENTATION

#include "rhi.hpp"
#include "core/exception.hpp"
#include "precompiled.h"

namespace ionengine::rhi
{
    auto VkResult_to_string(VkResult const result) -> std::string
    {
        switch (result)
        {
            case VK_NOT_READY:
                return "A fence or query has not yet completed";
            case VK_TIMEOUT:
                return "A wait operation has not completed in the specified time";
            case VK_EVENT_SET:
                return "An event is signaled";
            case VK_EVENT_RESET:
                return "An event is unsignaled";
            case VK_INCOMPLETE:
                return "A return array was too small for the result";
            default:
                return "An unknown error has occurred";
        }
    }

    auto TextureFormat_to_VkFormat(TextureFormat const format) -> VkFormat
    {
        switch (format)
        {
            case TextureFormat::Unknown:
                return VK_FORMAT_UNDEFINED;
            case TextureFormat::RGBA8_UNORM:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::BGRA8_UNORM:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::BGR8_UNORM:
                return VK_FORMAT_B8G8R8_UNORM;
            case TextureFormat::R8_UNORM:
                return VK_FORMAT_R8_UNORM;
            case TextureFormat::BC1:
                return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
            case TextureFormat::BC3:
                return VK_FORMAT_BC3_UNORM_BLOCK;
            case TextureFormat::BC4:
                return VK_FORMAT_BC4_UNORM_BLOCK;
            case TextureFormat::BC5:
                return VK_FORMAT_BC5_UNORM_BLOCK;
            case TextureFormat::D32_FLOAT:
                return VK_FORMAT_D32_SFLOAT;
            case TextureFormat::RGBA16_FLOAT:
                return VK_FORMAT_R16G16B16A16_SFLOAT;
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    auto TextureDimension_to_VkImageType(TextureDimension const dimension) -> VkImageType
    {
        switch (dimension)
        {
            case TextureDimension::_1D:
                return VkImageType::VK_IMAGE_TYPE_1D;
            case TextureDimension::Cube:
            case TextureDimension::_2DArray:
            case TextureDimension::_2D:
                return VkImageType::VK_IMAGE_TYPE_2D;
            case TextureDimension::_3D:
                return VkImageType::VK_IMAGE_TYPE_3D;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto throwIfFailed(VkResult const result) -> void
    {
        if (result != VK_SUCCESS)
        {
            throw core::Exception(VkResult_to_string(result));
        }
    }

    VKAPI_ATTR auto VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             void* pUserData) -> VkBool32
    {

        std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    VKTexture::VKTexture(VkDevice device, VmaAllocator memoryAllocator, TextureCreateInfo const& createInfo)
        : memoryAllocator(memoryAllocator), width(createInfo.width), height(createInfo.height), depth(createInfo.depth),
          mipLevels(createInfo.mipLevels), format(createInfo.format), dimension(createInfo.dimension),
          flags(createInfo.flags)
    {
        VkImageCreateInfo imageCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                          .imageType = TextureDimension_to_VkImageType(createInfo.dimension),
                                          .format = TextureFormat_to_VkFormat(createInfo.format),
                                          .extent = {.width = width, .height = height, .depth = depth},
                                          .mipLevels = mipLevels,
                                          .arrayLayers = depth,
                                          .samples = VK_SAMPLE_COUNT_1_BIT};

        if (flags & TextureUsage::DepthStencil)
        {
            imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if (flags & TextureUsage::RenderTarget)
        {
            imageCreateInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (flags & TextureUsage::UnorderedAccess)
        {
        }

        throwIfFailed(::vkCreateImage(device, &imageCreateInfo, nullptr, &image));

        VmaAllocationCreateInfo vmaAllocCreateInfo{.usage = VMA_MEMORY_USAGE_AUTO};
        throwIfFailed(
            ::vmaAllocateMemoryForImage(memoryAllocator, image, &vmaAllocCreateInfo, &memoryAllocation, nullptr));
    }

    VKTexture::VKTexture(VkDevice device, VkImage image, TextureCreateInfo const& createInfo)
        : memoryAllocator(nullptr), image(image), width(createInfo.width), height(createInfo.height),
          depth(createInfo.depth), mipLevels(createInfo.mipLevels), format(createInfo.format),
          dimension(createInfo.dimension), flags(createInfo.flags)
    {
    }

    VKTexture::~VKTexture()
    {
        if (memoryAllocation)
        {
            ::vmaDestroyImage(memoryAllocator, image, memoryAllocation);
        }
    }

    auto VKTexture::getWidth() const -> uint32_t
    {
        return width;
    }

    auto VKTexture::getHeight() const -> uint32_t
    {
        return height;
    }

    auto VKTexture::getDepth() const -> uint32_t
    {
        return depth;
    }

    auto VKTexture::getMipLevels() const -> uint32_t
    {
        return mipLevels;
    }

    auto VKTexture::getFormat() const -> TextureFormat
    {
        return format;
    }

    auto VKTexture::getFlags() const -> TextureUsageFlags
    {
        return flags;
    }

    auto VKTexture::getDescriptorOffset(TextureUsage const usage) const -> uint32_t
    {
        return 0;
    }

    VKGraphicsContext::VKGraphicsContext(VkDevice device, VkQueue queue, uint32_t queueFamilyIndex) : device(device)
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                                      .queueFamilyIndex = queueFamilyIndex};

        throwIfFailed(::vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
    }

    VKGraphicsContext::~VKGraphicsContext()
    {
        ::vkDestroyCommandPool(device, commandPool, nullptr);
    }

    auto VKGraphicsContext::reset() -> void
    {
        throwIfFailed(::vkResetCommandPool(device, commandPool, 0));
    }

    auto VKGraphicsContext::execute() -> Future<Query>
    {
        
    }

    auto VKGraphicsContext::barrier(core::ref_ptr<Buffer> dest, ResourceState const before,
                                    ResourceState const after) -> void
    {
    }

    auto VKGraphicsContext::barrier(core::ref_ptr<Texture> dest, ResourceState const before,
                                    ResourceState const after) -> void
    {
    }

    auto VKGraphicsContext::setGraphicsPipelineOptions(core::ref_ptr<Shader> shader,
                                                       RasterizerStageInfo const& rasterizer,
                                                       BlendColorInfo const& blendColor,
                                                       std::optional<DepthStencilStageInfo> const depthStencil) -> void
    {
    }

    auto VKGraphicsContext::bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void
    {
    }

    auto VKGraphicsContext::beginRenderPass(std::span<RenderPassColorInfo> const colors,
                                            std::optional<RenderPassDepthStencilInfo> depthStencil) -> void
    {
    }

    auto VKGraphicsContext::endRenderPass() -> void
    {
    }

    auto VKGraphicsContext::bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset,
                                             size_t const size) -> void
    {
    }

    auto VKGraphicsContext::bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                            IndexFormat const format) -> void
    {
    }

    auto VKGraphicsContext::drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void
    {
    }

    auto VKGraphicsContext::draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void
    {
    }

    auto VKGraphicsContext::setViewport(int32_t const x, int32_t const y, uint32_t const width,
                                        uint32_t const height) -> void
    {
    }

    auto VKGraphicsContext::setScissor(int32_t const left, int32_t const top, int32_t const right,
                                       int32_t const bottom) -> void
    {
    }

    VKDevice::VKDevice(RHICreateInfo const& createInfo)
    {
        VkApplicationInfo applicationInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                          .pApplicationName = "RHI",
                                          .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                          .pEngineName = "RHI",
                                          .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                          .apiVersion = VK_API_VERSION_1_2};
#ifdef _DEBUG
        std::array<char const*, 3> instanceExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_XLIB_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME};
        std::array<char const*, 1> instanceLayers = {"VK_LAYER_KHRONOS_validation"};

        VkInstanceCreateInfo instanceCreateInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                .pApplicationInfo = &applicationInfo,
                                                .enabledLayerCount = static_cast<uint32_t>(instanceLayers.size()),
                                                .ppEnabledLayerNames = instanceLayers.data(),
                                                .enabledExtensionCount =
                                                    static_cast<uint32_t>(instanceExtensions.size()),
                                                .ppEnabledExtensionNames = instanceExtensions.data()};

        throwIfFailed(::vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback};

        createDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

        destroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        throwIfFailed(createDebugUtilsMessengerEXT(instance, &messengerCreateInfo, nullptr, &debugUtilsMessenger));
#else
        std::array<char const*, 3> enabledExtensions = {VK_KHR_SURFACE_EXTENSION_NAME,
                                                        VK_KHR_XLIB_SURFACE_EXTENSION_NAME};

        VkInstanceCreateInfo instanceCreateInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                .pApplicationInfo = &applicationInfo,
                                                .enabledExtensionCount =
                                                    static_cast<uint32_t>(enabledExtensions.size()),
                                                .ppEnabledExtensionNames = enabledExtensions.data()};

        throwIfFailed(::vkCreateInstance(&instanceCreateInfo, nullptr, &instance));
#endif

        uint32_t numPhysicalDevices;
        throwIfFailed(::vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr));

        std::vector<VkPhysicalDevice> physicalDevices(numPhysicalDevices);
        throwIfFailed(::vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevices.data()));

        physicalDevice = physicalDevices[0];

        for (auto const& currentPhysicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties deviceProperties;
            ::vkGetPhysicalDeviceProperties(currentPhysicalDevice, &deviceProperties);
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                physicalDevice = currentPhysicalDevice;
                break;
            }
        }

        uint32_t numQueueFamilies;
        ::vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilies, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(numQueueFamilies);
        ::vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &numQueueFamilies, queueFamilies.data());

        float queuePriority = 0.0f;
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        int32_t graphicsQueueFamily = -1;
        int32_t transferQueueFamily = -1;
        int32_t computeQueueFamily = -1;

        for (uint32_t const i : std::views::iota(0u, queueFamilies.size()))
        {
            if (graphicsQueueFamily == -1 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                graphicsQueueFamily = i;
            }
            else if (transferQueueFamily == -1 && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                transferQueueFamily = i;
            }
            else if (computeQueueFamily == -1 && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                computeQueueFamily = i;
            }
            else
            {
                break;
            }

            VkDeviceQueueCreateInfo queueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                    .queueFamilyIndex = i,
                                                    .queueCount = 1,
                                                    .pQueuePriorities = &queuePriority};

            queueCreateInfos.emplace_back(std::move(queueCreateInfo));
        }

        std::array<char const*, 1> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        VkDeviceCreateInfo deviceCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                                            .pQueueCreateInfos = queueCreateInfos.data(),
                                            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
                                            .ppEnabledExtensionNames = deviceExtensions.data()};
        throwIfFailed(::vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

        ::vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue.queue);
        graphicsQueue.familyIndex = graphicsQueueFamily;

        ::vkGetDeviceQueue(device, transferQueueFamily, 0, &transferQueue.queue);
        transferQueue.familyIndex = transferQueueFamily;

        ::vkGetDeviceQueue(device, computeQueueFamily, 0, &computeQueue.queue);
        computeQueue.familyIndex = computeQueueFamily;

        if (createInfo.window)
        {
            VkXlibSurfaceCreateInfoKHR surfaceCreateInfo{.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
                                                         .dpy = reinterpret_cast<Display*>(createInfo.instance),
                                                         .window = reinterpret_cast<Window>(createInfo.window)};

            throwIfFailed(::vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));

            VkSwapchainCreateInfoKHR swapchainCreateInfo{
                .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                .surface = surface,
                .minImageCount = 2,
                .imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
                .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                .imageExtent = {.width = createInfo.windowWidth, .height = createInfo.windowHeight},
                .imageArrayLayers = 1,
                .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
                .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
                .presentMode = VK_PRESENT_MODE_FIFO_KHR,
                .clipped = true};

            throwIfFailed(::vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain));

            uint32_t numSwapchainImages;
            throwIfFailed(::vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, nullptr));

            std::vector<VkImage> swapchainImages(numSwapchainImages);
            throwIfFailed(::vkGetSwapchainImagesKHR(device, swapchain, &numSwapchainImages, swapchainImages.data()));

            for (auto const& swapchainImage : swapchainImages)
            {
                TextureCreateInfo textureCreateInfo{.width = swapchainCreateInfo.imageExtent.width,
                                                    .height = swapchainCreateInfo.imageExtent.height,
                                                    .depth = 1,
                                                    .mipLevels = 1,
                                                    .format = TextureFormat::BGRA8_UNORM,
                                                    .dimension = TextureDimension::_2D,
                                                    .flags = (TextureUsageFlags)TextureUsage::RenderTarget};

                auto texture = core::make_ref<VKTexture>(device, swapchainImage, textureCreateInfo);
                backBuffers.emplace_back(std::move(texture));
            }
        }
    }

    VKDevice::~VKDevice()
    {
        ::vkDestroySwapchainKHR(device, swapchain, nullptr);
        ::vkDestroySurfaceKHR(instance, surface, nullptr);
        ::vkDestroyDevice(device, nullptr);
#ifdef _DEBUG
        destroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, nullptr);
#endif
        ::vkDestroyInstance(instance, nullptr);
    }

    auto VKDevice::createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader>
    {
        return nullptr;
    }

    auto VKDevice::createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture>
    {
        return nullptr;
    }

    auto VKDevice::createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer>
    {
        return nullptr;
    }

    auto VKDevice::createGraphicsContext() -> core::ref_ptr<GraphicsContext>
    {
        return nullptr;
    }

    auto VKDevice::createCopyContext() -> core::ref_ptr<CopyContext>
    {
        return nullptr;
    }

    auto VKDevice::requestBackBuffer() -> core::ref_ptr<Texture>
    {
        return nullptr;
    }

    auto VKDevice::presentBackBuffer() -> void
    {
    }

    auto VKDevice::getBackendType() const -> std::string_view
    {
        return "Vulkan";
    }
} // namespace ionengine::rhi