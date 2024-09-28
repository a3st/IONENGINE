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

    auto throwIfFailed(VkResult const result) -> void
    {
        if (result != VK_SUCCESS)
        {
            throw core::runtime_error(VkResult_to_string(result));
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
                throw std::invalid_argument("Invalid argument for conversion");
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
                throw std::invalid_argument("Invalid argument for conversion");
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
                throw std::invalid_argument("Invalid argument for conversion");
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
                throw std::invalid_argument("Invalid argument for conversion");
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
                throw std::invalid_argument("Invalid argument for conversion");
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
                throw std::invalid_argument("Invalid argument for conversion");
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
                throw std::invalid_argument("Invalid argument for conversion");
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
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto BlendOp_to_D3D12_VkBlendOp(BlendOp const blendOp) -> VkBlendOp
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
                throw std::invalid_argument("Invalid argument for conversion");
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

    DescriptorAllocator::DescriptorAllocator(VkDevice device) : device(device)
    {
        std::array<VkDescriptorType, 4> descriptorTypes{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_DESCRIPTOR_TYPE_SAMPLER};
        std::array<VkDescriptorBindingFlags, 4> bindingFlags{};
        std::array<uint32_t, 4> descriptorLimits = {std::to_underlying(DescriptorAllocatorLimits::Uniform),
                                                    std::to_underlying(DescriptorAllocatorLimits::Storage),
                                                    std::to_underlying(DescriptorAllocatorLimits::SampledImage),
                                                    std::to_underlying(DescriptorAllocatorLimits::Sampler)};

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;

        for (uint32_t const i : std::views::iota(0u, 4u))
        {
            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{.binding = i,
                                                                    .descriptorType = descriptorTypes[i],
                                                                    .descriptorCount = descriptorLimits[i],
                                                                    .stageFlags = VK_SHADER_STAGE_ALL};
            descriptorSetLayoutBindings.emplace_back(std::move(descriptorSetLayoutBinding));
            bindingFlags[i] = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

            std::vector<uint8_t> free(descriptorLimits[i]);
            std::fill(free.begin(), free.end(), 0x0);

            std::vector<DescriptorAllocation_T> allocations(descriptorLimits[i]);

            Chunk chunk{.binding = i,
                        .free = std::move(free),
                        .size = descriptorLimits[i],
                        .allocations = std::move(allocations)};
            chunks.emplace(descriptorTypes[i], std::move(chunk));
        }

        VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount = static_cast<uint32_t>(bindingFlags.size()),
            .pBindingFlags = bindingFlags.data()};

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = &bindingFlagsCreateInfo,
            .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
            .bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size()),
            .pBindings = descriptorSetLayoutBindings.data()};
        throwIfFailed(
            ::vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout));
    }

    DescriptorAllocator::~DescriptorAllocator()
    {
        ::vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
    }

    auto DescriptorAllocator::allocate(VkDescriptorType const descriptorType,
                                       DescriptorAllocation* allocation) -> VkResult
    {
        std::lock_guard lock(mutex);

        Chunk& chunk = chunks[descriptorType];
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

            chunk.allocations[allocOffset] = {.descriptorType = descriptorType,
                                              .descriptorSet = nullptr,
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

    VKVertexInput::VKVertexInput(std::span<VertexDeclarationInfo const> const vertexDeclarations)
    {
        uint32_t location = 0;
        uint32_t offset = 0;

        for (auto const& vertexDeclaration : vertexDeclarations)
        {
            VkVertexInputAttributeDescription inputAttribute{.location = location,
                                                             .binding = 0,
                                                             .format =
                                                                 VertexFormat_to_VkFormat(vertexDeclaration.format),
                                                             .offset = offset};

            location++;
            offset += sizeof_VertexFormat(vertexDeclaration.format);

            inputAttributes.emplace_back(std::move(inputAttribute));
        }

        inputBinding =
            VkVertexInputBindingDescription{.binding = 0, .stride = offset, .inputRate = VK_VERTEX_INPUT_RATE_VERTEX};
    }

    auto VKVertexInput::getPipelineVertexInputState() const -> VkPipelineVertexInputStateCreateInfo
    {
        return VkPipelineVertexInputStateCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                                                    .vertexBindingDescriptionCount = 1,
                                                    .pVertexBindingDescriptions = &inputBinding,
                                                    .vertexAttributeDescriptionCount =
                                                        static_cast<uint32_t>(inputAttributes.size()),
                                                    .pVertexAttributeDescriptions = inputAttributes.data()};
    }

    VKShader::VKShader(VkDevice device, ShaderCreateInfo const& createInfo) : device(device)
    {
        if (createInfo.pipelineType == rhi::PipelineType::Graphics)
        {
            XXH64_state_t* hasher = ::XXH64_createState();
            {
                VkShaderModuleCreateInfo shaderModuleCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                    .pCode = reinterpret_cast<uint32_t const*>(createInfo.graphics.vertexStage.shader.data())};
                VkShaderModule shaderModule;
                throwIfFailed(::vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

                VKShaderStage shaderStage{.entryPoint = createInfo.graphics.vertexStage.entryPoint,
                                          .shaderModule = shaderModule};
                stages.emplace(VK_SHADER_STAGE_VERTEX_BIT, std::move(shaderStage));

                ::XXH64_update(hasher, createInfo.graphics.vertexStage.shader.data(),
                               createInfo.graphics.vertexStage.shader.size());
            }

            {
                VkShaderModuleCreateInfo shaderModuleCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                    .pCode = reinterpret_cast<uint32_t const*>(createInfo.graphics.pixelStage.shader.data())};
                VkShaderModule shaderModule;
                throwIfFailed(::vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

                VKShaderStage shaderStage{.entryPoint = createInfo.graphics.pixelStage.entryPoint,
                                          .shaderModule = shaderModule};
                stages.emplace(VK_SHADER_STAGE_FRAGMENT_BIT, std::move(shaderStage));

                ::XXH64_update(hasher, createInfo.graphics.pixelStage.shader.data(),
                               createInfo.graphics.pixelStage.shader.size());
            }
            hash = ::XXH64_digest(hasher);

            vertexInput.emplace(createInfo.graphics.vertexDeclarations);
        }
        else
        {
            XXH64_state_t* hasher = ::XXH64_createState();
            {
                VkShaderModuleCreateInfo shaderModuleCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                    .pCode = reinterpret_cast<uint32_t const*>(createInfo.compute.shader.data())};
                VkShaderModule shaderModule;
                throwIfFailed(::vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule));

                VKShaderStage shaderStage{.entryPoint = createInfo.compute.entryPoint, .shaderModule = shaderModule};
                stages.emplace(VK_SHADER_STAGE_COMPUTE_BIT, std::move(shaderStage));

                ::XXH64_update(hasher, createInfo.compute.shader.data(), createInfo.compute.shader.size());
            }
            hash = ::XXH64_digest(hasher);
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

    auto VKShader::getPipelineType() const -> PipelineType
    {
        return pipelineType;
    }

    auto VKShader::getStages() const -> std::unordered_map<VkShaderStageFlagBits, VKShaderStage> const&
    {
        return stages;
    }

    auto VKShader::getVertexInput() const -> std::optional<VKVertexInput>
    {
        return vertexInput;
    }

    Pipeline::Pipeline(VkDevice device, VkPipelineLayout pipelineLayout, VKShader* shader,
                       RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                       std::optional<DepthStencilStageInfo> const depthStencil,
                       std::span<VkFormat const> const renderTargetFormats, VkFormat const depthStencilFormat,
                       VkPipelineCache pipelineCache)
        : device(device)
    {
        if (shader->getPipelineType() == PipelineType::Graphics)
        {
            std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
            for (auto const [stageFlags, shaderStage] : shader->getStages())
            {
                VkPipelineShaderStageCreateInfo shaderStageCreateInfo{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                    .stage = stageFlags,
                    .module = shaderStage.shaderModule,
                    .pName = shaderStage.entryPoint.c_str()};

                shaderStageCreateInfos.emplace_back(std::move(shaderStageCreateInfo));
            }
            VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST};
            VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                .depthClampEnable = true,
                .polygonMode = FillMode_to_VkPolygonMode(rasterizer.fillMode),
                .cullMode = CullMode_to_VkCullModeFlags(rasterizer.cullMode),
                .frontFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .depthBiasEnable = true,
                .depthBiasConstantFactor = 0.0f,
                .depthBiasClamp = 0.0f,
                .depthBiasSlopeFactor = 0.0f,
                .lineWidth = 1.0f};
            VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};

            VkStencilOpState stencilOpState{.failOp = VK_STENCIL_OP_KEEP,
                                            .passOp = VK_STENCIL_OP_KEEP,
                                            .depthFailOp = VK_STENCIL_OP_KEEP,
                                            .compareOp = VK_COMPARE_OP_ALWAYS,
                                            .compareMask = 0xff,
                                            .writeMask = 0xff};
            auto depthStencilValue = depthStencil.value_or(DepthStencilStageInfo::Default());
            VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{
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
            VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            };

            std::vector<VkDynamicState> dynamicStates{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

            VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
                .pDynamicStates = dynamicStates.data()};
            VkPipelineRenderingCreateInfoKHR renderingCreateInfo{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR,
                .colorAttachmentCount = static_cast<uint32_t>(renderTargetFormats.size()),
                .pColorAttachmentFormats = renderTargetFormats.data(),
                .depthAttachmentFormat = depthStencilFormat,
                .stencilAttachmentFormat = depthStencilFormat};

            auto inputStateCreateInfo = shader->getVertexInput().value().getPipelineVertexInputState();
            VkGraphicsPipelineCreateInfo pipelineCreateInfo{.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                                                            .pNext = &renderingCreateInfo,
                                                            .stageCount =
                                                                static_cast<uint32_t>(shaderStageCreateInfos.size()),
                                                            .pStages = shaderStageCreateInfos.data(),
                                                            .pVertexInputState = &inputStateCreateInfo,
                                                            .pInputAssemblyState = &inputAssemblyCreateInfo,
                                                            .pRasterizationState = &rasterizationStateCreateInfo,
                                                            .pDepthStencilState = &depthStencilStateCreateInfo,
                                                            .pColorBlendState = &colorBlendStateCreateInfo,
                                                            .pDynamicState = &dynamicStateCreateInfo,
                                                            .layout = pipelineLayout};
            auto result =
                ::vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &pipeline);
            if (result != VK_SUCCESS)
            {
                throwIfFailed(::vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineCreateInfo, nullptr, &pipeline));
            }
        }
    }

    Pipeline::~Pipeline()
    {
        ::vkDestroyPipeline(device, pipeline, nullptr);
    }

    PipelineCache::PipelineCache(VkDevice device, RHICreateInfo const& createInfo) : device(device)
    {
        std::vector<VkPushConstantRange> pushContantRanges;
        for (uint32_t const i : std::views::iota(0u, 16u))
        {
            VkPushConstantRange pushConstantRange{.stageFlags = VK_SHADER_STAGE_ALL, .offset = i * 4, .size = 4};
            pushContantRanges.emplace_back(std::move(pushConstantRange));
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                                                            .pushConstantRangeCount =
                                                                static_cast<uint32_t>(pushContantRanges.size()),
                                                            .pPushConstantRanges = pushContantRanges.data()};
        throwIfFailed(::vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout));
    }

    auto PipelineCache::get(VKShader* shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                            std::optional<DepthStencilStageInfo> const depthStencil,
                            std::array<VkFormat, 8> const& renderTargetFormats,
                            VkFormat const depthStencilFormat) -> core::ref_ptr<Pipeline>
    {
        Entry entry{.shaderHash = shader->getHash(),
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

    VKBuffer::VKBuffer(VkDevice device, VmaAllocator memoryAllocator, BufferCreateInfo const& createInfo)
    {
    }

    VKBuffer::~VKBuffer()
    {
    }

    auto VKBuffer::getSize() -> size_t
    {
        return 0;
    }

    auto VKBuffer::getFlags() -> BufferUsageFlags
    {
        return 0;
    }

    auto VKBuffer::mapMemory() -> uint8_t*
    {
        return nullptr;
    }

    auto VKBuffer::unmapMemory() -> void
    {
    }

    auto VKBuffer::getBuffer() const -> VkBuffer
    {
        return nullptr;
    }

    auto VKBuffer::getDescriptorOffset(BufferUsage const usage) const -> uint32_t
    {
        return 0;
    }

    VKTexture::VKTexture(VkDevice device, VmaAllocator memoryAllocator, TextureCreateInfo const& createInfo)
        : device(device), memoryAllocator(memoryAllocator), memoryAllocation(nullptr), width(createInfo.width),
          height(createInfo.height), depth(createInfo.depth), mipLevels(createInfo.mipLevels),
          format(createInfo.format), dimension(createInfo.dimension), flags(createInfo.flags),
          initialLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR)
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

    VKTexture::VKTexture(VkDevice device, VkImage image, uint32_t const width, uint32_t const height)
        : device(device), memoryAllocator(nullptr), image(image), initialLayout(VK_IMAGE_LAYOUT_UNDEFINED),
          width(width), height(height), depth(1), mipLevels(1), format(TextureFormat::BGRA8_UNORM),
          dimension(TextureDimension::_2D), flags((TextureUsageFlags)TextureUsage::RenderTarget)
    {
        VkImageViewCreateInfo imageViewCreateInfo{.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
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

    auto VKTexture::getImage() const -> VkImage
    {
        return image;
    }

    auto VKTexture::getImageView() const -> VkImageView
    {
        return imageView;
    }

    auto VKTexture::getInitialLayout() const -> VkImageLayout
    {
        return initialLayout;
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
        VkSemaphoreWaitInfo semaphoreWaitInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
                                              .semaphoreCount = 1,
                                              .pSemaphores = &semaphore,
                                              .pValues = &fenceValue};
        throwIfFailed(::vkWaitSemaphores(device, &semaphoreWaitInfo, std::numeric_limits<uint64_t>::max()));
    }

    VKGraphicsContext::VKGraphicsContext(VkDevice device, VkQueue queue, uint32_t queueFamilyIndex,
                                         VkSemaphore semaphore, uint64_t& fenceValue)
        : device(device), queue(queue), queueFamilyIndex(queueFamilyIndex), semaphore(semaphore),
          fenceValue(&fenceValue)
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

        VkCommandBufferAllocateInfo commandBufferAllocInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                                           .commandPool = commandPool,
                                                           .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                                           .commandBufferCount = 1};
        throwIfFailed(::vkAllocateCommandBuffers(device, &commandBufferAllocInfo, &commandBuffer));

        VkCommandBufferBeginInfo commandBufferBeginInfo{.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                                        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT};
        throwIfFailed(::vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo));
    }

    auto VKGraphicsContext::execute() -> Future<Query>
    {
        throwIfFailed(::vkEndCommandBuffer(commandBuffer));

        (*fenceValue)++;
        VkTimelineSemaphoreSubmitInfo semaphoreSubmitInfo{.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR,
                                                          .signalSemaphoreValueCount = 1,
                                                          .pSignalSemaphoreValues = fenceValue};
        VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = &semaphoreSubmitInfo,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &commandBuffer,
                                .signalSemaphoreCount = 1,
                                .pSignalSemaphores = &semaphore};
        throwIfFailed(::vkQueueSubmit(queue, 1, &submitInfo, nullptr));

        auto query = core::make_ref<VKQuery>();
        auto futureImpl = std::make_unique<VKFutureImpl>(device, queue, semaphore, *fenceValue);
        return Future<Query>(query, std::move(futureImpl));
    }

    auto VKGraphicsContext::barrier(core::ref_ptr<Buffer> dest, ResourceState const before,
                                    ResourceState const after) -> void
    {
    }

    auto VKGraphicsContext::barrier(core::ref_ptr<Texture> dest, ResourceState const before,
                                    ResourceState const after) -> void
    {
        auto oldLayout = [&](ResourceState const state) -> VkImageLayout {
            VkImageLayout const initialLayout = static_cast<VKTexture*>(dest.get())->getInitialLayout();
            return state == ResourceState::Common && initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
                       ? initialLayout
                       : ResourceState_to_VkImageLayout(state);
        };

        auto newLayout = [&](ResourceState const state) -> VkImageLayout {
            VkImageLayout const initialLayout = static_cast<VKTexture*>(dest.get())->getInitialLayout();
            return state == ResourceState::Common && initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
                       ? VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                       : ResourceState_to_VkImageLayout(state);
        };

        VkImageMemoryBarrier imageMemoryBarrier{.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                .srcAccessMask = VkImageLayout_to_VkAccessFlags(oldLayout(before)),
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
        std::array<VkRenderingAttachmentInfo, 8> colorAttachmentInfos;
        for (uint32_t const i : std::views::iota(0u, colors.size()))
        {
            VkClearColorValue clearColorValue{};
            clearColorValue.float32[0] = colors[i].clearColor.r;
            clearColorValue.float32[1] = colors[i].clearColor.g;
            clearColorValue.float32[2] = colors[i].clearColor.b;
            clearColorValue.float32[3] = colors[i].clearColor.a;

            VkRenderingAttachmentInfo renderingAttachmentInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = static_cast<VKTexture*>(colors[i].texture.get())->getImageView(),
                .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                .loadOp = RenderPassLoadOp_to_VkAttachmentLoadOp(colors[i].loadOp),
                .storeOp = RenderPassStoreOp_to_VkAttachmentStoreOp(colors[i].storeOp),
                .clearValue = {.color = clearColorValue}};
            colorAttachmentInfos[i] = std::move(renderingAttachmentInfo);
        }

        VkRenderingInfo renderingInfo{.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
                                      .renderArea = renderArea,
                                      .layerCount = 1,
                                      .colorAttachmentCount = static_cast<uint32_t>(colors.size()),
                                      .pColorAttachments = colorAttachmentInfos.data()};
        ::vkCmdBeginRendering(commandBuffer, &renderingInfo);
    }

    auto VKGraphicsContext::endRenderPass() -> void
    {
        ::vkCmdEndRendering(commandBuffer);
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
        ::vkCmdDrawIndexed(commandBuffer, indexCount, instanceCount, 0, 0, 0);
    }

    auto VKGraphicsContext::draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void
    {
        ::vkCmdDraw(commandBuffer, vertexCount, instanceCount, 0, 0);
    }

    auto VKGraphicsContext::setViewport(int32_t const x, int32_t const y, uint32_t const width,
                                        uint32_t const height) -> void
    {
        VkViewport viewport{.x = static_cast<float>(x),
                            .y = static_cast<float>(y),
                            .width = static_cast<float>(width),
                            .height = static_cast<float>(height),
                            .minDepth = 0.0f,
                            .maxDepth = 1.0f};
        ::vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    }

    auto VKGraphicsContext::setScissor(int32_t const left, int32_t const top, int32_t const right,
                                       int32_t const bottom) -> void
    {
        VkRect2D rect{.offset = {.x = left, .y = top},
                      .extent = {.width = static_cast<uint32_t>(right), .height = static_cast<uint32_t>(bottom)}};
        ::vkCmdSetScissor(commandBuffer, 0, 1, &rect);
        renderArea = rect;
    }

    VKDevice::VKDevice(RHICreateInfo const& createInfo) : swapchain(nullptr)
    {
        VkApplicationInfo applicationInfo{.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
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
        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{
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

            VkDeviceQueueCreateInfo queueCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                                                    .queueFamilyIndex = i,
                                                    .queueCount = 1,
                                                    .pQueuePriorities = &queuePriority};
            queueCreateInfos.emplace_back(std::move(queueCreateInfo));
        }

        std::vector<char const*> deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                  VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                                                  VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};

        VkPhysicalDeviceVulkan12Features deviceFeatures12{.sType =
                                                              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                                                          .descriptorIndexing = true,
                                                          .descriptorBindingUniformBufferUpdateAfterBind = true,
                                                          .descriptorBindingSampledImageUpdateAfterBind = true,
                                                          .descriptorBindingStorageImageUpdateAfterBind = true,
                                                          .descriptorBindingStorageBufferUpdateAfterBind = true,
                                                          .descriptorBindingPartiallyBound = true,
                                                          .timelineSemaphore = true};
        VkPhysicalDeviceVulkan13Features deviceFeatures13{.sType =
                                                              VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
                                                          .pNext = &deviceFeatures12,
                                                          .dynamicRendering = true};
        VkDeviceCreateInfo deviceCreateInfo{.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
                                            .pNext = &deviceFeatures13,
                                            .queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size()),
                                            .pQueueCreateInfos = queueCreateInfos.data(),
                                            .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
                                            .ppEnabledExtensionNames = deviceExtensions.data()};
        throwIfFailed(::vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device));

        // Create Graphics Queue
        {
            ::vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue.queue);
            graphicsQueue.familyIndex = graphicsQueueFamily;

            VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                                              .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE};
            VkSemaphoreCreateInfo semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                      .pNext = &semaphoreTypeCreateInfo};
            throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &graphicsQueue.semaphore));
            graphicsQueue.fenceValue = 0;
        }

        // Create Transfer Queue
        {
            ::vkGetDeviceQueue(device, transferQueueFamily, 0, &transferQueue.queue);
            transferQueue.familyIndex = transferQueueFamily;

            VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                                              .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE};
            VkSemaphoreCreateInfo semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                      .pNext = &semaphoreTypeCreateInfo};
            throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &transferQueue.semaphore));
            transferQueue.fenceValue = 0;
        }

        // Create Compute Queue
        {
            ::vkGetDeviceQueue(device, computeQueueFamily, 0, &computeQueue.queue);
            computeQueue.familyIndex = computeQueueFamily;

            VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
                                                              .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE};
            VkSemaphoreCreateInfo semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                      .pNext = &semaphoreTypeCreateInfo};
            throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &computeQueue.semaphore));
            computeQueue.fenceValue = 0;
        }

        descriptorAllocator = core::make_ref<DescriptorAllocator>(device);

        if (createInfo.window)
        {
#ifdef IONENGINE_PLATFORM_WIN32
            VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
                                                          .hinstance = reinterpret_cast<HINSTANCE>(createInfo.instance),
                                                          .hwnd = reinterpret_cast<HWND>(createInfo.window)};
            throwIfFailed(::vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
#elif IONENGINE_PLATFORM_X11
            VkXlibSurfaceCreateInfoKHR surfaceCreateInfo{.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
                                                         .dpy = reinterpret_cast<Display*>(createInfo.instance),
                                                         .window = reinterpret_cast<Window>(createInfo.window)};
            throwIfFailed(::vkCreateXlibSurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface));
#endif

            {
                VkSemaphoreCreateInfo semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
                throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &acquireSemaphore));
            }

            {
                VkSemaphoreCreateInfo semaphoreCreateInfo{.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
                throwIfFailed(::vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &presentSemaphore));
            }

            this->createSwapchain(createInfo.windowWidth, createInfo.windowHeight);
        }
    }

    VKDevice::~VKDevice()
    {
        backBuffers.clear();
        if (swapchain)
        {
            ::vkDestroySemaphore(device, presentSemaphore, nullptr);
            ::vkDestroySemaphore(device, acquireSemaphore, nullptr);
            ::vkDestroySwapchainKHR(device, swapchain, nullptr);
            ::vkDestroySurfaceKHR(instance, surface, nullptr);
        }
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

    auto VKDevice::createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler>
    {
        return nullptr;
    }

    auto VKDevice::createGraphicsContext() -> core::ref_ptr<GraphicsContext>
    {
        return core::make_ref<VKGraphicsContext>(device, graphicsQueue.queue, graphicsQueue.familyIndex,
                                                 graphicsQueue.semaphore, graphicsQueue.fenceValue);
    }

    auto VKDevice::createCopyContext() -> core::ref_ptr<CopyContext>
    {
        return nullptr;
    }

    auto VKDevice::requestBackBuffer() -> core::ref_ptr<Texture>
    {
        if (!swapchain)
        {
            throw core::runtime_error("Swapchain is not found");
        }

        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        auto result = ::vkAcquireNextImageKHR(device, swapchain, std::numeric_limits<uint32_t>::max(), acquireSemaphore,
                                              nullptr, &imageIndex);
        if (result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR)
        {
            VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                    .waitSemaphoreCount = 1,
                                    .pWaitSemaphores = &acquireSemaphore,
                                    .pWaitDstStageMask = &waitDstStageMask};
            throwIfFailed(::vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, nullptr));
        }
        return backBuffers[imageIndex];
    }

    auto VKDevice::presentBackBuffer() -> void
    {
        if (!swapchain)
        {
            throw core::runtime_error("Swapchain is not found");
        }
        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkTimelineSemaphoreSubmitInfo semaphoreSubmitInfo{.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR,
                                                          .waitSemaphoreValueCount = 1,
                                                          .pWaitSemaphoreValues = &graphicsQueue.fenceValue};
        VkSubmitInfo submitInfo{.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = &semaphoreSubmitInfo,
                                .waitSemaphoreCount = 1,
                                .pWaitSemaphores = &graphicsQueue.semaphore,
                                .pWaitDstStageMask = &waitDstStageMask,
                                .signalSemaphoreCount = 1,
                                .pSignalSemaphores = &presentSemaphore};
        throwIfFailed(::vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, nullptr));

        VkPresentInfoKHR presentInfo{.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                                     .waitSemaphoreCount = 1,
                                     .pWaitSemaphores = &presentSemaphore,
                                     .swapchainCount = 1,
                                     .pSwapchains = &swapchain,
                                     .pImageIndices = &imageIndex};
        ::vkQueuePresentKHR(graphicsQueue.queue, &presentInfo);
    }

    auto VKDevice::resizeBackBuffers(uint32_t const width, uint32_t const height) -> void
    {
        throwIfFailed(::vkDeviceWaitIdle(device));
        backBuffers.clear();
        ::vkDestroySwapchainKHR(device, swapchain, nullptr);
        this->createSwapchain(width, height);
    }

    auto VKDevice::getBackendName() const -> std::string_view
    {
        return "Vulkan";
    }

    auto VKDevice::createSwapchain(uint32_t const width, uint32_t const height) -> void
    {
        VkSurfaceCapabilitiesKHR surfaceCabalities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCabalities);

        VkSwapchainCreateInfoKHR swapchainCreateInfo{.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
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
} // namespace ionengine::rhi