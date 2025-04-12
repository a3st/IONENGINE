// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#define VMA_IMPLEMENTATION

#include "vk.hpp"
#include "core/error.hpp"
#include "precompiled.h"

namespace ionengine::rhi
{
    auto VkResult_to_string(VkResult const result) -> std::string
    {
        switch (result)
        {
            case VK_NOT_READY:
                return "a fence or query has not yet completed";
            case VK_TIMEOUT:
                return "a wait operation has not completed in the specified time";
            case VK_EVENT_SET:
                return "an event is signaled";
            case VK_EVENT_RESET:
                return "an event is unsignaled";
            case VK_INCOMPLETE:
                return "a return array was too small for the result";
            default:
                return "an unknown error has occurred";
        }
    }

    auto throwIfFailed(VkResult const result) -> void
    {
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(VkResult_to_string(result));
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
                throw std::invalid_argument("unknown VkImageType for passed argument");
        }
    }

    auto RenderPassLoadOp_to_VkAttachmentLoadOp(RenderPassLoadOp const loadOp) -> VkAttachmentLoadOp
    {
        switch (loadOp)
        {
            case RenderPassLoadOp::Clear:
                return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case RenderPassLoadOp::Load:
                return VK_ATTACHMENT_LOAD_OP_LOAD;
            case RenderPassLoadOp::DontCare:
                return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            default:
                throw std::invalid_argument("unknown VkAttachmentLoadOp for passed argument");
        }
    }

    auto RenderPassStoreOp_to_VkAttachmentStoreOp(RenderPassStoreOp const storeOp) -> VkAttachmentStoreOp
    {
        switch (storeOp)
        {
            case RenderPassStoreOp::Store:
                return VK_ATTACHMENT_STORE_OP_STORE;
            case RenderPassStoreOp::DontCare:
                return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            default:
                throw std::invalid_argument("unknown VkAttachmentStoreOp for passed argument");
        }
    }

    auto ResourceState_to_VkImageLayout(ResourceState const state) -> VkImageLayout
    {
        switch (state)
        {
            case ResourceState::Common:
                return VK_IMAGE_LAYOUT_GENERAL;
            case ResourceState::DepthStencilRead:
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
            case ResourceState::DepthStencilWrite:
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case ResourceState::RenderTarget:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case ResourceState::UnorderedAccess:
                return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
            case ResourceState::ShaderRead:
                return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case ResourceState::CopySource:
                return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case ResourceState::CopyDest:
                return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            default:
                throw std::invalid_argument("unknown VkImageLayout for passed argument");
        }
    }

    auto VkImageLayout_to_VkAccessFlags(VkImageLayout const layout) -> VkAccessFlags
    {
        switch (layout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                return VK_ACCESS_NONE;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL:
                return VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                return VK_ACCESS_SHADER_READ_BIT;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                return VK_ACCESS_TRANSFER_READ_BIT;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                return VK_ACCESS_TRANSFER_WRITE_BIT;
            default:
                throw std::invalid_argument("unknown VkAccessFlags for passed argument");
        }
    }

    auto VertexFormat_to_VkFormat(VertexFormat const format) -> VkFormat
    {
        switch (format)
        {
            case VertexFormat::RGBA32_FLOAT:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case VertexFormat::RGBA32_SINT:
                return VK_FORMAT_R32G32B32A32_SINT;
            case VertexFormat::RGBA32_UINT:
                return VK_FORMAT_R32G32B32A32_UINT;
            case VertexFormat::RGB32_FLOAT:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case VertexFormat::RGB32_SINT:
                return VK_FORMAT_R32G32B32_SINT;
            case VertexFormat::RGB32_UINT:
                return VK_FORMAT_R32G32B32_UINT;
            case VertexFormat::RG32_FLOAT:
                return VK_FORMAT_R32G32_SFLOAT;
            case VertexFormat::RG32_SINT:
                return VK_FORMAT_R32G32_SINT;
            case VertexFormat::RG32_UINT:
                return VK_FORMAT_R32G32_UINT;
            case VertexFormat::R32_FLOAT:
                return VK_FORMAT_R32_SFLOAT;
            case VertexFormat::R32_SINT:
                return VK_FORMAT_R32_SINT;
            case VertexFormat::R32_UINT:
                return VK_FORMAT_R32_UINT;
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    auto CullMode_to_VkCullModeFlags(CullMode const cullMode) -> VkCullModeFlags
    {
        switch (cullMode)
        {
            case CullMode::Back:
                return VK_CULL_MODE_BACK_BIT;
            case CullMode::Front:
                return VK_CULL_MODE_FRONT_BIT;
            case CullMode::None:
                return VK_CULL_MODE_NONE;
            default:
                throw std::invalid_argument("unknown VkCullModeFlags for passed argument");
        }
    }

    auto FillMode_to_VkPolygonMode(FillMode const fillMode) -> VkPolygonMode
    {
        switch (fillMode)
        {
            case FillMode::Solid:
                return VK_POLYGON_MODE_FILL;
            case FillMode::Wireframe:
                return VK_POLYGON_MODE_LINE;
            default:
                throw std::invalid_argument("unknown VkPolygonMode for passed argument");
        }
    }

    auto CompareOp_to_VkCompareOp(CompareOp const compareOp) -> VkCompareOp
    {
        switch (compareOp)
        {
            case CompareOp::Always:
                return VK_COMPARE_OP_ALWAYS;
            case CompareOp::Equal:
                return VK_COMPARE_OP_EQUAL;
            case CompareOp::Greater:
                return VK_COMPARE_OP_GREATER;
            case CompareOp::GreaterEqual:
                return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case CompareOp::Less:
                return VK_COMPARE_OP_LESS;
            case CompareOp::LessEqual:
                return VK_COMPARE_OP_LESS_OR_EQUAL;
            case CompareOp::Never:
                return VK_COMPARE_OP_NEVER;
            case CompareOp::NotEqual:
                return VK_COMPARE_OP_NOT_EQUAL;
            default:
                throw std::invalid_argument("unknown VkCompareOp for passed argument");
        }
    }

    auto Blend_to_VkBlendFactor(Blend const blend) -> VkBlendFactor
    {
        switch (blend)
        {
            case Blend::InvSrcAlpha:
                return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            case Blend::One:
                return VK_BLEND_FACTOR_ONE;
            case Blend::SrcAlpha:
                return VK_BLEND_FACTOR_SRC_ALPHA;
            case Blend::Zero:
                return VK_BLEND_FACTOR_ZERO;
            default:
                throw std::invalid_argument("unknown VkBlendFactor for passed argument");
        }
    }

    auto BlendOp_to_VkBlendOp(BlendOp const blendOp) -> VkBlendOp
    {
        switch (blendOp)
        {
            case BlendOp::Add:
                return VK_BLEND_OP_ADD;
            case BlendOp::Max:
                return VK_BLEND_OP_MAX;
            case BlendOp::Min:
                return VK_BLEND_OP_MIN;
            case BlendOp::RevSubtract:
                return VK_BLEND_OP_REVERSE_SUBTRACT;
            case BlendOp::Subtract:
                return VK_BLEND_OP_SUBTRACT;
            default:
                throw std::invalid_argument("unknown VkBlendOp for passed argument");
        }
    }

    auto AddressMode_to_VkSamplerAddressMode(AddressMode const addressMode) -> VkSamplerAddressMode
    {
        switch (addressMode)
        {
            case AddressMode::Clamp:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case AddressMode::Mirror:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case AddressMode::Wrap:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            default:
                throw std::invalid_argument("unknown VkSamplerAddressMode for passed argument");
        }
    }

    VKAPI_ATTR auto VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
        -> VkBool32
    {
        std::cerr << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    DescriptorAllocator::DescriptorAllocator(VkDevice device) : device(device)
    {
        std::array<VkDescriptorType, 4> const descriptorTypes{
            VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
            VK_DESCRIPTOR_TYPE_SAMPLER};
        // std::array<VkDescriptorBindingFlags, 4> bindingFlags{};
        std::array<uint32_t, 4> const descriptorLimits{std::to_underlying(DescriptorAllocatorLimits::Uniform),
                                                       std::to_underlying(DescriptorAllocatorLimits::Storage),
                                                       std::to_underlying(DescriptorAllocatorLimits::SampledImage),
                                                       std::to_underlying(DescriptorAllocatorLimits::Sampler)};

        std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

        for (uint32_t const i : std::views::iota(0u, 4u))
        {
            VkDescriptorPoolSize const descriptorPoolSize{.type = descriptorTypes[i],
                                                          .descriptorCount = descriptorLimits[i]};
            descriptorPoolSizes.emplace_back(std::move(descriptorPoolSize));
        }

        VkDescriptorPoolCreateInfo const descriptorPoolCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
            .maxSets = 2,
            .poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size()),
            .pPoolSizes = descriptorPoolSizes.data()};
        throwIfFailed(::vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool));

        // bindingFlags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
        /*
        VkDescriptorSetLayoutBindingFlagsCreateInfo const bindingFlagsCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindingFlags.size()),
            .pBindingFlags = bindingFlags.data()};
        */
        {
            VkDescriptorSetLayoutBinding const descriptorSetLayoutBinding{.binding = 0,
                                                                          .descriptorType =
                                                                              VK_DESCRIPTOR_TYPE_MUTABLE_EXT,
                                                                          .descriptorCount = descriptorLimits[0],
                                                                          .stageFlags = VK_SHADER_STAGE_ALL};
            VkMutableDescriptorTypeListEXT const descriptorTypeList{.descriptorTypeCount = 3,
                                                                    .pDescriptorTypes = descriptorTypes.data()};
            VkMutableDescriptorTypeCreateInfoEXT const mutableDescriptorTypeCreateInfo{
                .sType = VK_STRUCTURE_TYPE_MUTABLE_DESCRIPTOR_TYPE_CREATE_INFO_EXT,
                .mutableDescriptorTypeListCount = 1,
                .pMutableDescriptorTypeLists = &descriptorTypeList};
            VkDescriptorSetLayoutCreateInfo const descriptorSetLayoutCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .pNext = &mutableDescriptorTypeCreateInfo,
                .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
                .bindingCount = 1,
                .pBindings = &descriptorSetLayoutBinding};

            VkDescriptorSetLayout descriptorSetLayout;
            throwIfFailed(
                ::vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

            this->createChunk(descriptorSetLayout, VK_DESCRIPTOR_TYPE_MUTABLE_EXT, descriptorLimits[0]);
        }

        {
            VkDescriptorSetLayoutBinding const descriptorSetLayoutBinding{.binding = 0,
                                                                          .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
                                                                          .descriptorCount = descriptorLimits[3],
                                                                          .stageFlags = VK_SHADER_STAGE_ALL};
            VkDescriptorSetLayoutCreateInfo const descriptorSetLayoutCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
                .bindingCount = 1,
                .pBindings = &descriptorSetLayoutBinding};

            VkDescriptorSetLayout descriptorSetLayout;
            throwIfFailed(
                ::vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));

            this->createChunk(descriptorSetLayout, VK_DESCRIPTOR_TYPE_SAMPLER, descriptorLimits[3]);
        }
    }

    auto DescriptorAllocator::createChunk(VkDescriptorSetLayout descriptorSetLayout,
                                          VkDescriptorType const descriptorType, uint32_t const descriptorCount) -> void
    {
        std::vector<uint8_t> free(descriptorCount);
        std::fill(free.begin(), free.end(), 0x0);

        std::vector<DescriptorAllocation_T> allocations(descriptorCount);

        Chunk chunk{.descriptorSetLayout = descriptorSetLayout,
                    .binding = 0,
                    .free = std::move(free),
                    .size = descriptorCount,
                    .allocations = std::move(allocations)};

        VkDescriptorSetAllocateInfo const descriptorSetAllocateInfo{.sType =
                                                                        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
                                                                    .descriptorPool = descriptorPool,
                                                                    .descriptorSetCount = 1,
                                                                    .pSetLayouts = &descriptorSetLayout};
        throwIfFailed(::vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &chunk.descriptorSet));

        chunks.emplace(descriptorType, std::move(chunk));
    }

    DescriptorAllocator::~DescriptorAllocator()
    {
        ::vkDestroyDescriptorPool(device, descriptorPool, nullptr);

        for (auto const [descriptorType, chunkData] : chunks)
        {
            ::vkDestroyDescriptorSetLayout(device, chunkData.descriptorSetLayout, nullptr);
        }
    }

    auto DescriptorAllocator::allocate(VkDescriptorType const descriptorType, DescriptorAllocation* allocation)
        -> VkResult
    {
        std::lock_guard lock(mutex);

        VkDescriptorType mutableDescriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        if (descriptorType != VK_DESCRIPTOR_TYPE_SAMPLER)
        {
            mutableDescriptorType = VK_DESCRIPTOR_TYPE_MUTABLE_EXT;
        }

        Chunk& chunk = chunks[mutableDescriptorType];
        if (1 > chunk.size - chunk.offset)
        {
            return VK_ERROR_OUT_OF_POOL_MEMORY;
        }

        bool success = false;
        uint32_t allocOffset = 0;

        for (uint32_t const i : std::views::iota(chunk.offset, chunk.size))
        {
            if (chunk.free[i] == 0x1)
            {
                continue;
            }
            else
            {
                allocOffset = i;
                success = true;
                break;
            }
        }

        if (success)
        {
            chunk.free[allocOffset] = 0x1;
            chunk.offset = allocOffset + 1;

            chunk.allocations[allocOffset] = {.descriptorType = mutableDescriptorType,
                                              .descriptorSet = chunk.descriptorSet,
                                              .binding = chunk.binding,
                                              .arrayElement = allocOffset};

            (*allocation) = &chunk.allocations[allocOffset];
            return VK_SUCCESS;
        }
        else
        {
            return VK_ERROR_OUT_OF_POOL_MEMORY;
        }
    }

    auto DescriptorAllocator::deallocate(DescriptorAllocation allocation) -> void
    {
        std::lock_guard lock(mutex);

        Chunk& chunk = chunks[allocation->descriptorType];
        chunk.free[allocation->arrayElement] = 0x0;
        chunk.offset = std::min(chunk.offset, allocation->arrayElement);
    }

    auto DescriptorAllocator::getDescriptorSet(VkDescriptorType const descriptorType) -> VkDescriptorSet
    {
        return chunks[descriptorType].descriptorSet;
    }

    auto DescriptorAllocator::getDescriptorSetLayout(VkDescriptorType const descriptorType) -> VkDescriptorSetLayout
    {
        return chunks[descriptorType].descriptorSetLayout;
    }

    VKVertexInput::VKVertexInput(std::span<VertexDeclarationInfo const> const vertexDeclarations)
        : vertexInputStateCreateInfo({.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO})
    {
        uint32_t location = 0;
        uint32_t offset = 0;

        for (auto const& vertexDeclaration : vertexDeclarations)
        {
            VkVertexInputAttributeDescription const vertexInputAttributeDescription{
                .location = location,
                .binding = 0,
                .format = VertexFormat_to_VkFormat(vertexDeclaration.format),
                .offset = offset};

            location++;
            offset += sizeof_VertexFormat(vertexDeclaration.format);

            inputAttributes.emplace_back(std::move(vertexInputAttributeDescription));
        }

        if (!inputAttributes.empty())
        {
            inputBinding = VkVertexInputBindingDescription{
                .binding = 0, .stride = offset, .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};

            vertexInputStateCreateInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                                          .vertexBindingDescriptionCount = 1,
                                          .pVertexBindingDescriptions = &inputBinding,
                                          .vertexAttributeDescriptionCount =
                                              static_cast<uint32_t>(inputAttributes.size()),
                                          .pVertexAttributeDescriptions = inputAttributes.data()};
        }
    }

    auto VKVertexInput::getPipelineVertexInputState() const -> VkPipelineVertexInputStateCreateInfo const&
    {
        return vertexInputStateCreateInfo;
    }

    VKShader::VKShader(VkDevice device, ShaderCreateInfo const& createInfo) : device(device)
    {
        if (createInfo.shaderType == rhi::ShaderType::Graphics)
        {
            XXH64_state_t* hasher = ::XXH64_createState();
            ::XXH64_reset(hasher, 0);

            {
                VkShaderModuleCreateInfo const shaderModuleCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                    .codeSize = createInfo.graphics.vertexStage.shaderCode.size(),
                    .pCode = reinterpret_cast<uint32_t const*>(createInfo.graphics.vertexStage.shaderCode.data())};
                VkShaderModule shaderModule;
                throwIfFailed(::vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

                VKShaderStage const shaderStage{.entryPoint = createInfo.graphics.vertexStage.entryPoint,
                                                .shaderModule = shaderModule};
                stages.emplace(VK_SHADER_STAGE_VERTEX_BIT, std::move(shaderStage));

                ::XXH64_update(hasher, createInfo.graphics.vertexStage.shaderCode.data(),
                               createInfo.graphics.vertexStage.shaderCode.size());
            }

            {
                VkShaderModuleCreateInfo const shaderModuleCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                    .codeSize = createInfo.graphics.pixelStage.shaderCode.size(),
                    .pCode = reinterpret_cast<uint32_t const*>(createInfo.graphics.pixelStage.shaderCode.data())};
                VkShaderModule shaderModule;
                throwIfFailed(::vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

                VKShaderStage const shaderStage{.entryPoint = createInfo.graphics.pixelStage.entryPoint,
                                                .shaderModule = shaderModule};
                stages.emplace(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(shaderStage));

                ::XXH64_update(hasher, createInfo.graphics.pixelStage.shaderCode.data(),
                               createInfo.graphics.pixelStage.shaderCode.size());
            }

            hash = ::XXH64_digest(hasher);
            ::XXH64_freeState(hasher);

            vertexInput.emplace(createInfo.graphics.vertexDeclarations);
        }
        else
        {
            {
                VkShaderModuleCreateInfo const shaderModuleCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                    .codeSize = createInfo.compute.shaderCode.size(),
                    .pCode = reinterpret_cast<uint32_t const*>(createInfo.compute.shaderCode.data())};
                VkShaderModule shaderModule;
                throwIfFailed(::vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

                VKShaderStage const shaderStage{.entryPoint = createInfo.compute.entryPoint,
                                                .shaderModule = shaderModule};
                stages.emplace(VK_SHADER_STAGE_COMPUTE_BIT, std::move(shaderStage));
            }

            hash = ::XXH64(createInfo.compute.shaderCode.data(), createInfo.compute.shaderCode.size(), 0);
        }
    }

    VKShader::~VKShader()
    {
        for (auto const [stageFlags, shaderStage] : stages)
        {
            ::vkDestroyShaderModule(device, shaderStage.shaderModule, nullptr);
        }
    }

    auto VKShader::getHash() const -> uint64_t
    {
        return hash;
    }

    auto VKShader::getShaderType() const -> ShaderType
    {
        return shaderType;
    }

    auto VKShader::getStages() const -> std::unordered_map<VkShaderStageFlagBits, VKShaderStage> const&
    {
        return stages;
    }

    auto VKShader::getVertexInput() const -> std::optional<VKVertexInput> const&
    {
        return vertexInput;
    }

    Pipeline::Pipeline(VkDevice device, VkPipelineLayout pipelineLayout, VKShader* shader,
                       RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                       std::optional<DepthStencilStageInfo> const depthStencil,
                       std::array<VkFormat, 8> const& renderTargetFormats, VkFormat const depthStencilFormat,
                       VkPipelineCache pipelineCache)
        : device(device), pipelineType(VK_PIPELINE_BIND_POINT_GRAPHICS)
    {
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
        for (auto const& [stageFlags, shaderStage] : shader->getStages())
        {
            VkPipelineShaderStageCreateInfo const shaderStageCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = stageFlags,
                .module = shaderStage.shaderModule,
                .pName = shaderStage.entryPoint.c_str()};

            shaderStageCreateInfos.emplace_back(std::move(shaderStageCreateInfo));
        }

        VkPipelineInputAssemblyStateCreateInfo const inputAssemblyCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};

        VkPipelineRasterizationStateCreateInfo const rasterizationStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = false,
            .polygonMode = FillMode_to_VkPolygonMode(rasterizer.fillMode),
            .cullMode = CullMode_to_VkCullModeFlags(rasterizer.cullMode),
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = true,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0f};

        VkStencilOpState const stencilOpState{.failOp = VK_STENCIL_OP_KEEP,
                                              .passOp = VK_STENCIL_OP_KEEP,
                                              .depthFailOp = VK_STENCIL_OP_KEEP,
                                              .compareOp = VK_COMPARE_OP_ALWAYS,
                                              .compareMask = 0xff,
                                              .writeMask = 0xff};

        auto depthStencilValue = depthStencil.value_or(DepthStencilStageInfo::Default());
        VkPipelineDepthStencilStateCreateInfo const depthStencilStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = depthStencilValue.depthWrite,
            .depthWriteEnable = depthStencilValue.depthWrite,
            .depthCompareOp = CompareOp_to_VkCompareOp(depthStencilValue.depthFunc),
            .depthBoundsTestEnable = depthStencilValue.depthWrite,
            .stencilTestEnable = depthStencilValue.stencilWrite,
            .front = stencilOpState,
            .back = stencilOpState,
            .minDepthBounds = 0.0f,
            .maxDepthBounds = 1.0f};

        VkPipelineColorBlendAttachmentState const colorBlendAttachmentState{
            .blendEnable = blendColor.blendEnable,
            .srcColorBlendFactor = Blend_to_VkBlendFactor(blendColor.blendSrc),
            .dstColorBlendFactor = Blend_to_VkBlendFactor(blendColor.blendDst),
            .colorBlendOp = BlendOp_to_VkBlendOp(blendColor.blendOp),
            .srcAlphaBlendFactor = Blend_to_VkBlendFactor(blendColor.blendSrcAlpha),
            .dstAlphaBlendFactor = Blend_to_VkBlendFactor(blendColor.blendDstAlpha),
            .alphaBlendOp = BlendOp_to_VkBlendOp(blendColor.blendOpAlpha),
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                              VK_COLOR_COMPONENT_A_BIT};

        std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentStates;
        for (uint32_t const i : std::views::iota(0u, renderTargetFormats.size()))
        {
            if (renderTargetFormats[i] == VK_FORMAT_UNDEFINED)
            {
                break;
            }

            colorBlendAttachmentStates.emplace_back(colorBlendAttachmentState);
        }

        VkPipelineColorBlendStateCreateInfo const colorBlendStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size()),
            .pAttachments = colorBlendAttachmentStates.data()};

        std::array<VkDynamicState, 2> const dynamicStates{VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT,
                                                          VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT};
        VkPipelineDynamicStateCreateInfo const dynamicStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
            .pDynamicStates = dynamicStates.data()};

        VkPipelineRenderingCreateInfoKHR const renderingCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
            .colorAttachmentCount = static_cast<uint32_t>(colorBlendAttachmentStates.size()),
            .pColorAttachmentFormats = renderTargetFormats.data(),
            .depthAttachmentFormat = depthStencilFormat,
            .stencilAttachmentFormat = depthStencilFormat};

        VkPipelineMultisampleStateCreateInfo const multisampleStateCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = false};

        auto vertexInputStateCreateInfo = shader->getVertexInput().value().getPipelineVertexInputState();
        VkGraphicsPipelineCreateInfo const pipelineCreateInfo{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                                              .pNext = &renderingCreateInfo,
                                                              .stageCount =
                                                                  static_cast<uint32_t>(shaderStageCreateInfos.size()),
                                                              .pStages = shaderStageCreateInfos.data(),
                                                              .pVertexInputState = &vertexInputStateCreateInfo,
                                                              .pInputAssemblyState = &inputAssemblyCreateInfo,
                                                              .pRasterizationState = &rasterizationStateCreateInfo,
                                                              .pMultisampleState = &multisampleStateCreateInfo,
                                                              .pDepthStencilState = &depthStencilStateCreateInfo,
                                                              .pColorBlendState = &colorBlendStateCreateInfo,
                                                              .pDynamicState = &dynamicStateCreateInfo,
                                                              .layout = pipelineLayout};
        auto result = ::vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline);
        if (result != VK_SUCCESS)
        {
            throwIfFailed(::vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline));
        }
    }

    Pipeline::~Pipeline()
    {
        ::vkDestroyPipeline(device, pipeline, nullptr);
    }

    auto Pipeline::getPipelineType() const -> VkPipelineBindPoint
    {
        return pipelineType;
    }

    auto Pipeline::getPipeline() -> VkPipeline
    {
        return pipeline;
    }

    PipelineCache::PipelineCache(VkDevice device, DescriptorAllocator* descriptorAllocator,
                                 RHICreateInfo const& createInfo)
        : device(device)
    {
        VkPushConstantRange const pushConstantRange{.stageFlags = VK_SHADER_STAGE_ALL, .offset = 0, .size = 4 * 16};

        std::array<VkDescriptorSetLayout, 2> const descriptorSetLayouts{
            descriptorAllocator->getDescriptorSetLayout(VK_DESCRIPTOR_TYPE_MUTABLE_EXT),
            descriptorAllocator->getDescriptorSetLayout(VK_DESCRIPTOR_TYPE_SAMPLER)};
        VkPipelineLayoutCreateInfo const pipelineLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()),
            .pSetLayouts = descriptorSetLayouts.data(),
            .pushConstantRangeCount = 1,
            .pPushConstantRanges = &pushConstantRange};
        throwIfFailed(::vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
    }

    auto PipelineCache::get(VKShader* shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                            std::optional<DepthStencilStageInfo> const depthStencil,
                            std::array<VkFormat, 8> const& renderTargetFormats, VkFormat const depthStencilFormat)
        -> core::ref_ptr<Pipeline>
    {
        Entry const entry{.shaderHash = shader->getHash(),
                          .rasterizer = rasterizer,
                          .blendColor = blendColor,
                          .depthStencil = depthStencil,
                          .renderTargetFormats = renderTargetFormats,
                          .depthStencilFormat = depthStencilFormat};

        std::unique_lock lock(mutex);
        auto result = entries.find(entry);
        if (result != entries.end())
        {
            return result->second;
        }
        else
        {
            lock.unlock();
            auto pipeline = core::make_ref<Pipeline>(device, pipelineLayout, shader, rasterizer, blendColor,
                                                     depthStencil, renderTargetFormats, depthStencilFormat, nullptr);

            lock.lock();
            entries.emplace(entry, pipeline);
            return pipeline;
        }
    }

    PipelineCache::~PipelineCache()
    {
        ::vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }

    auto PipelineCache::reset() -> void
    {
        std::lock_guard lock(mutex);
        entries.clear();
    }

    auto PipelineCache::getPipelineLayout() -> VkPipelineLayout
    {
        return pipelineLayout;
    }

    VKBuffer::VKBuffer(VkDevice device, VmaAllocator memoryAllocator, BufferCreateInfo const& createInfo)
        : device(device), memoryAllocator(memoryAllocator), size(createInfo.size), flags(createInfo.flags)
    {
        VkBufferCreateInfo bufferCreateInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    }

    VKBuffer::~VKBuffer()
    {
        if (memoryAllocator && memoryAllocation)
        {
            ::vmaDestroyBuffer(memoryAllocator, buffer, memoryAllocation);
        }
    }

    auto VKBuffer::getSize() const -> size_t
    {
        return size;
    }

    auto VKBuffer::getFlags() const -> BufferUsageFlags
    {
        return flags;
    }

    auto VKBuffer::mapMemory() -> uint8_t*
    {
        uint8_t* mappedBytes;
        if (flags & BufferUsage::MapWrite || flags & BufferUsage::MapRead)
        {
            throwIfFailed(::vmaMapMemory(memoryAllocator, memoryAllocation, reinterpret_cast<void**>(&mappedBytes)));
        }
        return mappedBytes;
    }

    auto VKBuffer::unmapMemory() -> void
    {
        ::vmaUnmapMemory(memoryAllocator, memoryAllocation);
    }

    auto VKBuffer::getBuffer() -> VkBuffer
    {
        return buffer;
    }

    auto VKBuffer::getDescriptorOffset(BufferUsage const usage) const -> uint32_t
    {
        return 0;
    }

    VKTexture::VKTexture(VkDevice device, VmaAllocator memoryAllocator, DescriptorAllocator* descriptorAllocator,
                         TextureCreateInfo const& createInfo)
        : device(device), memoryAllocator(memoryAllocator), descriptorAllocator(descriptorAllocator),
          memoryAllocation(nullptr), width(createInfo.width), height(createInfo.height), depth(createInfo.depth),
          mipLevels(createInfo.mipLevels), format(createInfo.format), dimension(createInfo.dimension),
          flags(createInfo.flags), initialLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
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
            imageCreateInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        }
        if (flags & TextureUsage::RenderTarget)
        {
            imageCreateInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (flags & TextureUsage::UnorderedAccess)
        {
            imageCreateInfo.usage |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (flags & TextureUsage::ShaderResource)
        {
            imageCreateInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        if (flags & TextureUsage::CopyDest)
        {
            imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }
        if (flags & TextureUsage::CopySource)
        {
            imageCreateInfo.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }

        VmaAllocationCreateInfo allocationCreateInfo{.usage = VMA_MEMORY_USAGE_AUTO};
        throwIfFailed(::vmaCreateImage(memoryAllocator, &imageCreateInfo, &allocationCreateInfo, &image,
                                       &memoryAllocation, nullptr));

        VkImageViewCreateInfo const imageViewCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                                        .image = image,
                                                        .viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
                                                        .format = TextureFormat_to_VkFormat(format),
                                                        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                       .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                       .a = VK_COMPONENT_SWIZZLE_IDENTITY},
                                                        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                                             .baseMipLevel = 0,
                                                                             .levelCount = mipLevels,
                                                                             .baseArrayLayer = 0,
                                                                             .layerCount = depth}};
        throwIfFailed(::vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView));

        if (flags & TextureUsage::ShaderResource)
        {
            assert(descriptorAllocator && "to create a texture with views, you need to pass the allocator descriptor");

            DescriptorAllocation descriptorAllocation;
            throwIfFailed(descriptorAllocator->allocate(VK_DESCRIPTOR_TYPE_MUTABLE_EXT, &descriptorAllocation));
            descriptorAllocations.emplace(TextureUsage::ShaderResource, descriptorAllocation);

            // TODO! Write
        }
    }

    VKTexture::VKTexture(VkDevice device, VkImage image, uint32_t const width, uint32_t const height)
        : device(device), memoryAllocator(nullptr), image(image), initialLayout(VK_IMAGE_LAYOUT_UNDEFINED),
          width(width), height(height), depth(1), mipLevels(1), format(TextureFormat::BGRA8_UNORM),
          dimension(TextureDimension::_2D), flags((TextureUsageFlags)TextureUsage::RenderTarget)
    {
        VkImageViewCreateInfo const imageViewCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                                        .image = image,
                                                        .viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
                                                        .format = TextureFormat_to_VkFormat(format),
                                                        .components = {.r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                       .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                       .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                                                       .a = VK_COMPONENT_SWIZZLE_IDENTITY},
                                                        .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                                             .baseMipLevel = 0,
                                                                             .levelCount = mipLevels,
                                                                             .baseArrayLayer = 0,
                                                                             .layerCount = depth}};
        throwIfFailed(::vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView));
    }

    VKTexture::~VKTexture()
    {
        ::vkDestroyImageView(device, imageView, nullptr);
        if (memoryAllocator && memoryAllocation)
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

    auto VKTexture::getImage() -> VkImage
    {
        return image;
    }

    auto VKTexture::getImageView() -> VkImageView
    {
        return imageView;
    }

    auto VKTexture::getInitialLayout() const -> VkImageLayout
    {
        return initialLayout;
    }

    VKSampler::VKSampler(VkDevice device, SamplerCreateInfo const& createInfo) : device(device)
    {
        VkSamplerCreateInfo const samplerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = AddressMode_to_VkSamplerAddressMode(createInfo.addressU),
            .addressModeV = AddressMode_to_VkSamplerAddressMode(createInfo.addressV),
            .addressModeW = AddressMode_to_VkSamplerAddressMode(createInfo.addressW),
            .anisotropyEnable = true,
            .maxAnisotropy = static_cast<float>(createInfo.maxAnisotropy),
            .compareEnable = true,
            .compareOp = CompareOp_to_VkCompareOp(createInfo.compareOp),
            .maxLod = VK_LOD_CLAMP_NONE};

        ::vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler);
    }

    VKSampler::~VKSampler()
    {
        ::vkDestroySampler(device, sampler, nullptr);
    }

    auto VKSampler::getDescriptorOffset() const -> uint32_t
    {
        return 0;
    }

    VKFutureImpl::VKFutureImpl(VkDevice device, VkQueue queue, VkSemaphore semaphore, uint64_t const fenceValue)
        : device(device), queue(queue), semaphore(semaphore), fenceValue(fenceValue)
    {
    }

    auto VKFutureImpl::getResult() const -> bool
    {
        uint64_t counterValue;
        throwIfFailed(::vkGetSemaphoreCounterValue(device, semaphore, &counterValue));
        return counterValue >= fenceValue;
    }

    auto VKFutureImpl::wait() -> void
    {
        VkSemaphoreWaitInfo const semaphoreWaitInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                                                    .semaphoreCount = 1,
                                                    .pSemaphores = &semaphore,
                                                    .pValues = &fenceValue};
        throwIfFailed(::vkWaitSemaphores(device, &semaphoreWaitInfo, std::numeric_limits<uint64_t>::max()));
    }

    auto VKDeviceContext_barrier(VkCommandBuffer commandBuffer, core::ref_ptr<Buffer> dest, ResourceState const before,
                                 ResourceState const after) -> void
    {
    }

    auto VKDeviceContext_barrier(VkCommandBuffer commandBuffer, core::ref_ptr<Texture> dest, ResourceState const before,
                                 ResourceState const after) -> void
    {
        auto vkDestTexture = static_cast<VKTexture*>(dest.get());

        auto oldLayout = [&](ResourceState const state) -> VkImageLayout {
            VkImageLayout const initialLayout = vkDestTexture->getInitialLayout();
            return state == ResourceState::Common && initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
                       ? initialLayout
                       : ResourceState_to_VkImageLayout(state);
        };

        auto newLayout = [&](ResourceState const state) -> VkImageLayout {
            VkImageLayout const initialLayout = vkDestTexture->getInitialLayout();
            return state == ResourceState::Common && initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
                       ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                       : ResourceState_to_VkImageLayout(state);
        };

        VkImageMemoryBarrier const imageMemoryBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                      .srcAccessMask =
                                                          VkImageLayout_to_VkAccessFlags(oldLayout(before)),
                                                      .dstAccessMask = VkImageLayout_to_VkAccessFlags(newLayout(after)),
                                                      .oldLayout = oldLayout(before),
                                                      .newLayout = newLayout(after),
                                                      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                      .image = static_cast<VKTexture*>(dest.get())->getImage(),
                                                      .subresourceRange = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                                                           .baseMipLevel = 0,
                                                                           .levelCount = dest->getMipLevels(),
                                                                           .baseArrayLayer = 0,
                                                                           .layerCount = dest->getDepth()}};
        ::vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                               VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
    }

    VKGraphicsContext::VKGraphicsContext(VkDevice device, PipelineCache* pipelineCache,
                                         DescriptorAllocator* descriptorAllocator, DeviceQueueData& deviceQueue)
        : device(device), pipelineCache(pipelineCache), descriptorAllocator(descriptorAllocator),
          deviceQueue(&deviceQueue), isCommandListOpened(false)
    {
        VkCommandPoolCreateInfo const commandPoolCreateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                                            .queueFamilyIndex = deviceQueue.familyIndex};
        throwIfFailed(::vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));

        bindingData.resize(sizeof(uint32_t) * 4 * 16);
    }

    VKGraphicsContext::~VKGraphicsContext()
    {
        ::vkDestroyCommandPool(device, commandPool, nullptr);
    }

    auto VKGraphicsContext::tryAllocateCommandBuffer() -> void
    {
        if (isCommandListOpened)
        {
            return;
        }

        throwIfFailed(::vkResetCommandPool(device, commandPool, 0));

        VkCommandBufferAllocateInfo const commandBufferAllocInfo{.sType =
                                                                     VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                                 .commandPool = commandPool,
                                                                 .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                 .commandBufferCount = 1};
        throwIfFailed(::vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

        VkCommandBufferBeginInfo const commandBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                              .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
        throwIfFailed(::vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

        std::array<VkDescriptorSet, 2> const descriptorSets{
            descriptorAllocator->getDescriptorSet(VK_DESCRIPTOR_TYPE_MUTABLE_EXT),
            descriptorAllocator->getDescriptorSet(VK_DESCRIPTOR_TYPE_SAMPLER)};
        ::vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineCache->getPipelineLayout(), 0,
                                  2, descriptorSets.data(), 0, nullptr);

        isCommandListOpened = true;
        renderArea = {};
    }

    auto VKGraphicsContext::execute() -> Future<void>
    {
        throwIfFailed(::vkEndCommandBuffer(commandBuffer));
        isCommandListOpened = false;

        deviceQueue->fenceValue++;
        VkTimelineSemaphoreSubmitInfo const semaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR,
            .signalSemaphoreValueCount = 1,
            .pSignalSemaphoreValues = &deviceQueue->fenceValue};
        VkSubmitInfo const submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .pNext = &semaphoreSubmitInfo,
                                      .commandBufferCount = 1,
                                      .pCommandBuffers = &commandBuffer,
                                      .signalSemaphoreCount = 1,
                                      .pSignalSemaphores = &deviceQueue->semaphore};
        throwIfFailed(::vkQueueSubmit(deviceQueue->queue, 1, &submitInfo, nullptr));

        auto futureImpl =
            std::make_unique<VKFutureImpl>(device, deviceQueue->queue, deviceQueue->semaphore, deviceQueue->fenceValue);
        return Future<void>(std::move(futureImpl));
    }

    auto VKGraphicsContext::setGraphicsPipelineOptions(core::ref_ptr<Shader> shader,
                                                       RasterizerStageInfo const& rasterizer,
                                                       BlendColorInfo const& blendColor,
                                                       std::optional<DepthStencilStageInfo> const depthStencil) -> void
    {
        this->tryAllocateCommandBuffer();

        auto vkShader = dynamic_cast<VKShader*>(shader.get());

        auto pipeline =
            pipelineCache->get(vkShader, rasterizer, blendColor, depthStencil, renderTargetFormats, depthStencilFormat);

        ::vkCmdBindPipeline(commandBuffer, pipeline->getPipelineType(), pipeline->getPipeline());
    }

    auto VKGraphicsContext::bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void
    {
        std::memcpy(bindingData.data() + index * sizeof(uint32_t) * 4, &descriptor, sizeof(uint32_t));
    }

    auto VKGraphicsContext::beginRenderPass(std::span<RenderPassColorInfo const> const colors,
                                            std::optional<RenderPassDepthStencilInfo> depthStencil) -> void
    {
        this->tryAllocateCommandBuffer();

        renderTargetFormats.fill(VK_FORMAT_UNDEFINED);
        depthStencilFormat = VK_FORMAT_UNDEFINED;

        std::array<VkRenderingAttachmentInfo, 8> colorAttachmentInfos;
        for (uint32_t const i : std::views::iota(0u, colors.size()))
        {
            auto vkTexture = dynamic_cast<VKTexture*>(colors[i].texture.get());
            renderTargetFormats[i] = TextureFormat_to_VkFormat(vkTexture->getFormat());

            VkRenderingAttachmentInfo const renderingAttachmentInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = vkTexture->getImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .loadOp = RenderPassLoadOp_to_VkAttachmentLoadOp(colors[i].loadOp),
                .storeOp = RenderPassStoreOp_to_VkAttachmentStoreOp(colors[i].storeOp),
                .clearValue = {.color = {.float32 = {colors[i].clearColor.r, colors[i].clearColor.g,
                                                     colors[i].clearColor.b, colors[i].clearColor.a}}}};
            colorAttachmentInfos[i] = std::move(renderingAttachmentInfo);
        }

        VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
                                      .renderArea = renderArea,
                                      .layerCount = 1,
                                      .colorAttachmentCount = static_cast<uint32_t>(colors.size()),
                                      .pColorAttachments = colorAttachmentInfos.data()};

        if (depthStencil.has_value())
        {
            auto value = depthStencil.value();

            auto vkTexture = dynamic_cast<VKTexture*>(value.texture.get());
            depthStencilFormat = TextureFormat_to_VkFormat(vkTexture->getFormat());

            VkRenderingAttachmentInfo const depthAttachmentInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = vkTexture->getImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .loadOp = RenderPassLoadOp_to_VkAttachmentLoadOp(value.depthLoadOp),
                .storeOp = RenderPassStoreOp_to_VkAttachmentStoreOp(value.depthStoreOp),
                .clearValue = {.depthStencil = {.depth = value.clearDepth}}};
            renderingInfo.pDepthAttachment = &depthAttachmentInfo;

            VkRenderingAttachmentInfo const stencilAttachmentInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = vkTexture->getImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
                .loadOp = RenderPassLoadOp_to_VkAttachmentLoadOp(value.stencilLoadOp),
                .storeOp = RenderPassStoreOp_to_VkAttachmentStoreOp(value.stencilStoreOp),
                .clearValue = {.depthStencil = {.stencil = value.clearStencil}}};
            renderingInfo.pStencilAttachment = &stencilAttachmentInfo;

            ::vkCmdBeginRendering(commandBuffer, &renderingInfo);
        }
        else
        {
            ::vkCmdBeginRendering(commandBuffer, &renderingInfo);
        }
    }

    auto VKGraphicsContext::endRenderPass() -> void
    {
        ::vkCmdEndRendering(commandBuffer);
    }

    auto VKGraphicsContext::bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
        -> void
    {
        auto vkBuffer = dynamic_cast<VKBuffer*>(buffer.get());

        std::array<VkBuffer, 1> const buffers{vkBuffer->getBuffer()};
        ::vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers.data(), &offset);
    }

    auto VKGraphicsContext::bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                            IndexFormat const format) -> void
    {
        auto vkBuffer = dynamic_cast<VKBuffer*>(buffer.get());

        VkIndexType indexType;
        switch (format)
        {
            case IndexFormat::Uint32: {
                indexType = VkIndexType::VK_INDEX_TYPE_UINT32;
                break;
            }
            case IndexFormat::Uint16: {
                indexType = VkIndexType::VK_INDEX_TYPE_UINT16;
                break;
            }
        }

        ::vkCmdBindIndexBuffer(commandBuffer, vkBuffer->getBuffer(), offset, indexType);
    }

    auto VKGraphicsContext::drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void
    {
        ::vkCmdPushConstants(commandBuffer, pipelineCache->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0, 16,
                             bindingData.data());
        ::vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, 0);
    }

    auto VKGraphicsContext::draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void
    {
        ::vkCmdPushConstants(commandBuffer, pipelineCache->getPipelineLayout(), VK_SHADER_STAGE_ALL, 0, 16,
                             bindingData.data());
        ::vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
    }

    auto VKGraphicsContext::setViewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
        -> void
    {
        this->tryAllocateCommandBuffer();

        VkViewport const viewport{.x = static_cast<float>(x),
                                  .y = static_cast<float>(y),
                                  .width = static_cast<float>(width),
                                  .height = static_cast<float>(height),
                                  .minDepth = 0.0f,
                                  .maxDepth = 1.0f};
        ::vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    auto VKGraphicsContext::setScissor(int32_t const left, int32_t const top, int32_t const right, int32_t const bottom)
        -> void
    {
        this->tryAllocateCommandBuffer();

        VkRect2D const rect{.offset = {.x = left, .y = top},
                            .extent = {.width = static_cast<uint32_t>(right), .height = static_cast<uint32_t>(bottom)}};
        ::vkCmdSetScissor(commandBuffer, 0, 1, &rect);

        renderArea = rect;
    }

    auto VKGraphicsContext::barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
        -> void
    {
        this->tryAllocateCommandBuffer();

        VKDeviceContext_barrier(commandBuffer, dest, before, after);
    }

    auto VKGraphicsContext::barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
        -> void
    {
        this->tryAllocateCommandBuffer();

        VKDeviceContext_barrier(commandBuffer, dest, before, after);
    }

    auto VKCopyContext::tryAllocateCommandBuffer() -> void
    {
        if (isCommandListOpened)
        {
            return;
        }

        throwIfFailed(::vkResetCommandPool(device, commandPool, 0));

        VkCommandBufferAllocateInfo const commandBufferAllocInfo{.sType =
                                                                     VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                                 .commandPool = commandPool,
                                                                 .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                                 .commandBufferCount = 1};
        throwIfFailed(::vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

        VkCommandBufferBeginInfo const commandBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                              .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
        throwIfFailed(::vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));

        isCommandListOpened = true;
    }

    VKCopyContext::VKCopyContext(VkDevice device, VmaAllocator memoryAllocator, DeviceQueueData& deviceQueue,
                                 RHICreateInfo const& rhiCreateInfo)
        : device(device), deviceQueue(&deviceQueue), isCommandListOpened(false)
    {
        VkCommandPoolCreateInfo const commandPoolCreateInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                                            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
                                                            .queueFamilyIndex = deviceQueue.familyIndex};
        throwIfFailed(::vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool));
    }

    VKCopyContext::~VKCopyContext()
    {
        ::vkDestroyCommandPool(device, commandPool, nullptr);
    }

    auto VKCopyContext::updateBuffer(core::ref_ptr<Buffer> dest, uint64_t const offset,
                                     std::span<uint8_t const> const dataBytes) -> Future<Buffer>
    {
        return Future<Buffer>();
    }

    auto VKCopyContext::updateTexture(core::ref_ptr<Texture> dest, uint32_t const resourceIndex,
                                      std::span<uint8_t const> const dataBytes) -> Future<Texture>
    {
        return Future<Texture>();
    }

    auto VKCopyContext::barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
        -> void
    {
        this->tryAllocateCommandBuffer();

        VKDeviceContext_barrier(commandBuffer, dest, before, after);
    }

    auto VKCopyContext::barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
        -> void
    {
        this->tryAllocateCommandBuffer();

        VKDeviceContext_barrier(commandBuffer, dest, before, after);
    }

    auto VKCopyContext::execute() -> Future<void>
    {
        writeStagingBuffer.offset = 0;

        throwIfFailed(::vkEndCommandBuffer(commandBuffer));
        isCommandListOpened = false;

        deviceQueue->fenceValue++;
        VkTimelineSemaphoreSubmitInfo const semaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR,
            .signalSemaphoreValueCount = 1,
            .pSignalSemaphoreValues = &deviceQueue->fenceValue};
        VkSubmitInfo const submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .pNext = &semaphoreSubmitInfo,
                                      .commandBufferCount = 1,
                                      .pCommandBuffers = &commandBuffer,
                                      .signalSemaphoreCount = 1,
                                      .pSignalSemaphores = &deviceQueue->semaphore};
        throwIfFailed(::vkQueueSubmit(deviceQueue->queue, 1, &submitInfo, nullptr));

        auto futureImpl =
            std::make_unique<VKFutureImpl>(device, deviceQueue->queue, deviceQueue->semaphore, deviceQueue->fenceValue);
        return Future<void>(std::move(futureImpl));
    }

    VKSwapchain::VKSwapchain(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device,
                             DeviceQueueData& deviceQueue, SwapchainCreateInfo const& createInfo)
        : instance(instance), physicalDevice(physicalDevice), device(device), deviceQueue(&deviceQueue)
    {
#ifdef IONENGINE_PLATFORM_WIN32
        VkWin32SurfaceCreateInfoKHR const surfaceCreateInfo{.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                                                            .hinstance =
                                                                reinterpret_cast<HINSTANCE>(createInfo.instance),
                                                            .hwnd = reinterpret_cast<HWND>(createInfo.window)};
        throwIfFailed(::vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
#elif IONENGINE_PLATFORM_X11
        VkXlibSurfaceCreateInfoKHR const surfaceCreateInfo{.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
                                                           .dpy = reinterpret_cast<Display*>(createInfo.instance),
                                                           .window = reinterpret_cast<Window>(createInfo.window)};
        throwIfFailed(::vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
#endif

        VkSemaphoreCreateInfo const semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
        throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &acquireSemaphore));
        throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &presentSemaphore));

        RECT rect{};
        ::GetClientRect(reinterpret_cast<HWND>(createInfo.window), &rect);

        this->createSwapchainBuffers(rect.right, rect.bottom);
    }

    VKSwapchain::~VKSwapchain()
    {
        ::vkDeviceWaitIdle(device);
        backBuffers.clear();
        ::vkDestroySemaphore(device, presentSemaphore, nullptr);
        ::vkDestroySemaphore(device, acquireSemaphore, nullptr);
        ::vkDestroySwapchainKHR(device, swapchain, nullptr);
        ::vkDestroySurfaceKHR(instance, surface, nullptr);
    }

    auto VKSwapchain::getBackBuffer() -> core::weak_ptr<Texture>
    {
        VkPipelineStageFlags const waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        auto result = ::vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint32_t>::max(), acquireSemaphore,
                                              nullptr, &imageIndex);
        if (result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR)
        {
            VkSubmitInfo const submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                          .waitSemaphoreCount = 1,
                                          .pWaitSemaphores = &acquireSemaphore,
                                          .pWaitDstStageMask = &waitDstStageMask};
            throwIfFailed(::vkQueueSubmit(deviceQueue->queue, 1, &submitInfo, nullptr));
        }
        return backBuffers[imageIndex];
    }

    auto VKSwapchain::presentBackBuffer() -> void
    {
        VkPipelineStageFlags const waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkTimelineSemaphoreSubmitInfo const semaphoreSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR,
            .waitSemaphoreValueCount = 1,
            .pWaitSemaphoreValues = &deviceQueue->fenceValue};
        VkSubmitInfo const submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                      .pNext = &semaphoreSubmitInfo,
                                      .waitSemaphoreCount = 1,
                                      .pWaitSemaphores = &deviceQueue->semaphore,
                                      .pWaitDstStageMask = &waitDstStageMask,
                                      .signalSemaphoreCount = 1,
                                      .pSignalSemaphores = &presentSemaphore};
        throwIfFailed(::vkQueueSubmit(deviceQueue->queue, 1, &submitInfo, nullptr));

        VkPresentInfoKHR const presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                           .waitSemaphoreCount = 1,
                                           .pWaitSemaphores = &presentSemaphore,
                                           .swapchainCount = 1,
                                           .pSwapchains = &swapchain,
                                           .pImageIndices = &imageIndex};
        ::vkQueuePresentKHR(deviceQueue->queue, &presentInfo);
    }

    auto VKSwapchain::resizeBackBuffers(uint32_t const width, uint32_t const height) -> void
    {
        throwIfFailed(::vkDeviceWaitIdle(device));
        backBuffers.clear();
        ::vkDestroySwapchainKHR(device, swapchain, nullptr);
        this->createSwapchainBuffers(width, height);
    }

    auto VKSwapchain::createSwapchainBuffers(uint32_t const width, uint32_t const height) -> void
    {
        VkSurfaceCapabilitiesKHR surfaceCabalities;
        throwIfFailed(::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCabalities));

        VkSwapchainCreateInfoKHR const swapchainCreateInfo{.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                                                           .surface = surface,
                                                           .minImageCount = 2,
                                                           .imageFormat = VK_FORMAT_B8G8R8A8_UNORM,
                                                           .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
                                                           .imageExtent = {.width = width, .height = height},
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
            auto texture = core::make_ref<VKTexture>(device, swapchainImage, swapchainCreateInfo.imageExtent.width,
                                                     swapchainCreateInfo.imageExtent.height);
            backBuffers.emplace_back(std::move(texture));
        }
    }

    VKRHI::VKRHI(RHICreateInfo const& createInfo)
    {
        VkApplicationInfo const applicationInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                                .pApplicationName = "RHI",
                                                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                                .pEngineName = "RHI",
                                                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                                .apiVersion = VK_API_VERSION_1_3};

        VkInstanceCreateInfo instanceCreateInfo{.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                                .pApplicationInfo = &applicationInfo};

        std::vector<char const*> instanceExtensions{VK_KHR_SURFACE_EXTENSION_NAME};

#ifdef IONENGINE_PLATFORM_WIN32
        instanceExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif IONENGINE_PLATFORM_X11
        instanceExtensions.emplace_back{VK_KHR_XLIB_SURFACE_EXTENSION_NAME};
#endif

#ifndef NDEBUG
        instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        std::vector<char const*> instanceLayers{"VK_LAYER_KHRONOS_validation"};

        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
        instanceCreateInfo.ppEnabledLayerNames = instanceLayers.data();
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
#else
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
#endif
        throwIfFailed(::vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

#ifndef NDEBUG
        VkDebugUtilsMessengerCreateInfoEXT const messengerCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity =
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback};

        createDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            ::vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

        destroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            ::vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));

        throwIfFailed(createDebugUtilsMessengerEXT(instance, &messengerCreateInfo, nullptr, &debugUtilsMessenger));
#endif

        uint32_t numPhysicalDevices;
        throwIfFailed(::vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, nullptr));

        std::vector<VkPhysicalDevice> physicalDevices(numPhysicalDevices);
        throwIfFailed(::vkEnumeratePhysicalDevices(instance, &numPhysicalDevices, physicalDevices.data()));

        for (auto const& currentPhysicalDevice : physicalDevices)
        {
            VkPhysicalDeviceProperties deviceProperties;
            ::vkGetPhysicalDeviceProperties(currentPhysicalDevice, &deviceProperties);
            if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
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
            if (transferQueueFamily == -1 && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                transferQueueFamily = i;
            }
            if (computeQueueFamily == -1 && queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                computeQueueFamily = i;
            }

            VkDeviceQueueCreateInfo const queueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                          .queueFamilyIndex = i,
                                                          .queueCount = 1,
                                                          .pQueuePriorities = &queuePriority};
            queueCreateInfos.emplace_back(std::move(queueCreateInfo));
        }

        std::vector<char const*> const deviceExtensions{
            VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, VK_EXT_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME,
            VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME};

        VkPhysicalDeviceMutableDescriptorTypeFeaturesEXT deviceMutableFeatures{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MUTABLE_DESCRIPTOR_TYPE_FEATURES_EXT,
            .mutableDescriptorType = true};
        VkPhysicalDeviceFeatures const deviceFeatures{.samplerAnisotropy = true};
        VkPhysicalDeviceVulkan12Features deviceFeatures12{.sType =
                                                              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                                                          .pNext = &deviceMutableFeatures,
                                                          .descriptorIndexing = true,
                                                          .descriptorBindingUniformBufferUpdateAfterBind = true,
                                                          .descriptorBindingSampledImageUpdateAfterBind = true,
                                                          .descriptorBindingStorageImageUpdateAfterBind = true,
                                                          .descriptorBindingStorageBufferUpdateAfterBind = true,
                                                          .descriptorBindingPartiallyBound = true,
                                                          .runtimeDescriptorArray = true,
                                                          .timelineSemaphore = true};
        VkPhysicalDeviceVulkan13Features deviceFeatures13{.sType =
                                                              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
                                                          .pNext = &deviceFeatures12,
                                                          .dynamicRendering = true};
        VkDeviceCreateInfo const deviceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = &deviceFeatures13,
            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
            .ppEnabledExtensionNames = deviceExtensions.data(),
            .pEnabledFeatures = &deviceFeatures};
        throwIfFailed(::vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

        this->createDeviceQueue(graphicsQueueFamily, graphicsQueue);
        this->createDeviceQueue(transferQueueFamily, transferQueue);
        this->createDeviceQueue(computeQueueFamily, computeQueue);

        descriptorAllocator = core::make_ref<DescriptorAllocator>(device);

        VmaAllocatorCreateInfo const memoryAllocatorCreateInfo{
            .physicalDevice = physicalDevice, .device = device, .instance = instance};
        ::vmaCreateAllocator(&memoryAllocatorCreateInfo, &memoryAllocator);

        pipelineCache = core::make_ref<PipelineCache>(device, descriptorAllocator.get(), createInfo);

        graphicsContext =
            core::make_ref<VKGraphicsContext>(device, pipelineCache.get(), descriptorAllocator.get(), graphicsQueue);
        copyContext = core::make_ref<VKCopyContext>(device, memoryAllocator, transferQueue, createInfo);
    }

    VKRHI::~VKRHI()
    {
        ::vkDeviceWaitIdle(device);
        swapchain = nullptr;
        copyContext = nullptr;
        graphicsContext = nullptr;
        pipelineCache = nullptr;
        ::vmaDestroyAllocator(memoryAllocator);
        descriptorAllocator = nullptr;
        ::vkDestroySemaphore(device, graphicsQueue.semaphore, nullptr);
        ::vkDestroySemaphore(device, transferQueue.semaphore, nullptr);
        ::vkDestroySemaphore(device, computeQueue.semaphore, nullptr);
        ::vkDestroyDevice(device, nullptr);
#ifndef NDEBUG
        destroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, nullptr);
#endif
        ::vkDestroyInstance(instance, nullptr);
    }

    auto VKRHI::createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader>
    {
        return core::make_ref<VKShader>(device, createInfo);
    }

    auto VKRHI::createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture>
    {
        return core::make_ref<VKTexture>(device, memoryAllocator, descriptorAllocator.get(), createInfo);
    }

    auto VKRHI::createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer>
    {
        return core::make_ref<VKBuffer>(device, memoryAllocator, createInfo);
    }

    auto VKRHI::createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler>
    {
        return core::make_ref<VKSampler>(device, createInfo);
    }

    auto VKRHI::tryGetSwapchain(SwapchainCreateInfo const& createInfo) -> Swapchain*
    {
        if (!swapchain)
        {
            swapchain = core::make_ref<VKSwapchain>(instance, physicalDevice, device, graphicsQueue, createInfo);
        }
        return swapchain.get();
    }

    auto VKRHI::getGraphicsContext() -> GraphicsContext*
    {
        return graphicsContext.get();
    }

    auto VKRHI::getCopyContext() -> CopyContext*
    {
        return copyContext.get();
    }

    auto VKRHI::getName() const -> std::string const&
    {
        return rhiName;
    }

    auto VKRHI::createDeviceQueue(uint32_t const queueFamily, DeviceQueueData& deviceQueue) -> void
    {
        ::vkGetDeviceQueue(device, queueFamily, 0, &deviceQueue.queue);
        deviceQueue.familyIndex = queueFamily;

        VkSemaphoreTypeCreateInfo const semaphoreTypeCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                                                .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE};
        VkSemaphoreCreateInfo const semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                        .pNext = &semaphoreTypeCreateInfo};
        throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &deviceQueue.semaphore));

        uint64_t counterValue;
        throwIfFailed(::vkGetSemaphoreCounterValue(device, deviceQueue.semaphore, &counterValue));
        deviceQueue.fenceValue = counterValue;
    }
} // namespace ionengine::rhi