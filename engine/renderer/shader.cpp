// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader.hpp"
#include "precompiled.h"

namespace ionengine
{
    auto FXVertexFormat_to_RHIVertexFormat(shadersys::fx::VertexFormat const format) -> rhi::VertexFormat
    {
        switch (format)
        {
            case shadersys::fx::VertexFormat::RGBA32_FLOAT:
                return rhi::VertexFormat::RGBA32_FLOAT;
            case shadersys::fx::VertexFormat::RGBA32_SINT:
                return rhi::VertexFormat::RGBA32_SINT;
            case shadersys::fx::VertexFormat::RGBA32_UINT:
                return rhi::VertexFormat::RGBA32_UINT;
            case shadersys::fx::VertexFormat::RGB32_FLOAT:
                return rhi::VertexFormat::RGB32_FLOAT;
            case shadersys::fx::VertexFormat::RGB32_SINT:
                return rhi::VertexFormat::RGB32_SINT;
            case shadersys::fx::VertexFormat::RGB32_UINT:
                return rhi::VertexFormat::RGB32_UINT;
            case shadersys::fx::VertexFormat::RG32_FLOAT:
                return rhi::VertexFormat::RG32_FLOAT;
            case shadersys::fx::VertexFormat::RG32_SINT:
                return rhi::VertexFormat::RG32_SINT;
            case shadersys::fx::VertexFormat::RG32_UINT:
                return rhi::VertexFormat::RG32_UINT;
            case shadersys::fx::VertexFormat::R32_FLOAT:
                return rhi::VertexFormat::R32_FLOAT;
            case shadersys::fx::VertexFormat::R32_SINT:
                return rhi::VertexFormat::R32_SINT;
            case shadersys::fx::VertexFormat::R32_UINT:
                return rhi::VertexFormat::R32_UINT;
            default:
                throw std::invalid_argument("Passed invalid argument into function");
        }
    }

    Shader::Shader(rhi::Device& device, shadersys::ShaderFile const& shaderFile)
    {
        std::string apiType;
        switch (shaderFile.apiType)
        {
            case shadersys::fx::APIType::DXIL: {
                apiType = "D3D12";
                break;
            }
            case shadersys::fx::APIType::SPIRV: {
                apiType = "Vulkan";
                break;
            }
        }

        if (apiType != device.getBackendName())
        {
            throw core::runtime_error("Shader has a different shader backend");
        }

        rhi::ShaderCreateInfo shaderCreateInfo{.pipelineType = rhi::PipelineType::Graphics};

        /*for (auto const& [stageType, stageData] : shaderFile.shaderData.outputData.stages)
        {
            uint32_t const bufferIndex = stageData.buffer;
            auto const& bufferData = shaderFile.shaderData.buffers[bufferIndex];

            if (stageType == shadersys::fx::StageType::Compute)
            {
                shaderCreateInfo.pipelineType = rhi::PipelineType::Compute;

                rhi::ShaderStageCreateInfo stageCreateInfo{
                    .entryPoint = stageData.entryPoint,
                    .shader = {shaderFile.blob.data() + bufferData.offset, bufferData.size}};
                shaderCreateInfo.compute = std::move(stageCreateInfo);
            }
            else
            {
                rhi::ShaderStageCreateInfo stageCreateInfo{
                    .entryPoint = stageData.entryPoint,
                    .shader = {shaderFile.blob.data() + bufferData.offset, bufferData.size}};
                switch (stageType)
                {
                    case shadersys::fx::StageType::Vertex: {
                        for (auto const& inputElement : stageData.vertexLayout.elements)
                        {
                            rhi::VertexDeclarationInfo vertexDeclarationInfo{
                                .semantic = inputElement.semantic,
                                .format = FXVertexFormat_to_RHIVertexFormat(inputElement.format)};

                            shaderCreateInfo.graphics.vertexDeclarations.emplace_back(std::move(vertexDeclarationInfo));
                        }

                        shaderCreateInfo.graphics.vertexStage = std::move(stageCreateInfo);
                        break;
                    }
                    case shadersys::fx::StageType::Pixel: {
                        rasterizerStageInfo.fillMode = rhi::FillMode::Solid;

                        switch (shaderFile.shaderData.outputData.cullSide)
                        {
                            case shadersys::fx::CullSide::Back: {
                                rasterizerStageInfo.cullMode = rhi::CullMode::Back;
                                break;
                            }
                            case shadersys::fx::CullSide::Front: {
                                rasterizerStageInfo.cullMode = rhi::CullMode::Front;
                                break;
                            }
                            case shadersys::fx::CullSide::None: {
                                rasterizerStageInfo.cullMode = rhi::CullMode::None;
                                break;
                            }
                        }

                        shaderCreateInfo.graphics.pixelStage = std::move(stageCreateInfo);
                        break;
                    }
                }
            }
        }

        shaderProgram = device.createShader(shaderCreateInfo);*/
    }

    auto Shader::setActive(rhi::GraphicsContext& context, uint32_t const flags) -> void
    {
        auto result = shaderVariants.find(flags);
        if (result == shaderVariants.end())
        {
        }

        context.setGraphicsPipelineOptions(result->second.shaderProgram, rasterizerStageInfo,
                                           rhi::BlendColorInfo::Opaque(), std::nullopt);
    }

    auto Shader::getFlagsByName(std::string_view const permutationName) const -> uint32_t
    {
        auto result = permutationNames.find(std::string(permutationName));
        if (result == permutationNames.end())
        {
        }
        return result->second;
    }

    /*
    auto ShaderAsset::parseShaderEffectFile(rhi::fx::ShaderEffectFile const& shaderEffectFile) -> bool
    {
        std::string targetType;
        switch (shaderEffectFile.target)
        {
            case rhi::fx::ShaderTargetType::DXIL: {
                targetType = "D3D12";
                break;
            }
            case rhi::fx::ShaderTargetType::SPIRV: {
                targetType = "Vulkan";
                break;
            }
        }

        if (targetType != device->getDevice().getBackendType())
        {
            std::cerr << "[Renderer] ShaderAsset {} has a different shader backend" << std::endl;
            return false;
        }

        if (shaderEffectFile.effectData.technique.stages.find(rhi::fx::ShaderStageType::Compute) !=
            shaderEffectFile.effectData.technique.stages.end())
        {
            uint32_t const bufferIndex =
                shaderEffectFile.effectData.technique.stages.at(rhi::fx::ShaderStageType::Compute).buffer;
            shaderProgram = device->getDevice().createShader(shaderEffectFile.buffers[bufferIndex]);
        }
        else
        {
            uint32_t const vertexBufferIndex =
                shaderEffectFile.effectData.technique.stages.at(rhi::fx::ShaderStageType::Vertex).buffer;
            uint32_t const pixelBufferIndex =
                shaderEffectFile.effectData.technique.stages.at(rhi::fx::ShaderStageType::Pixel).buffer;

            rasterizerStage.fillMode = rhi::FillMode::Solid;

            switch (shaderEffectFile.effectData.technique.cullSide)
            {
                case rhi::fx::ShaderCullSide::Back: {
                    rasterizerStage.cullMode = rhi::CullMode::Back;
                    break;
                }
                case rhi::fx::ShaderCullSide::Front: {
                    rasterizerStage.cullMode = rhi::CullMode::Front;
                    break;
                }
                case rhi::fx::ShaderCullSide::None: {
                    rasterizerStage.cullMode = rhi::CullMode::None;
                    break;
                }
            }

            auto found = std::find_if(shaderEffectFile.effectData.structures.begin(),
                                      shaderEffectFile.effectData.structures.end(),
                                      [](auto const& element) { return element.structureName == "VS_INPUT"; });
            if (found == shaderEffectFile.effectData.structures.end())
            {
                std::cerr << "[Renderer] ShaderAsset {} hasn't input assembler and cannot be used as a vertex
    shader"
                          << std::endl;
                return false;
            }

            std::vector<rhi::VertexDeclarationInfo> vertexDeclarations;
            uint32_t index = 0;
            for (auto const& structureElement : found->elements)
            {
                rhi::VertexDeclarationInfo vertexDeclarationInfo = {
                    .semantic = structureElement.semantic,
                    .index = index,
                    .format = ShaderElementType_to_VertexFormat(structureElement.elementType)};

                vertexDeclarations.emplace_back(std::move(vertexDeclarationInfo));
                ++index;
            }

            shaderProgram =
                device->getDevice().createShader(vertexDeclarations, shaderEffectFile.buffers[vertexBufferIndex],
                                                 shaderEffectFile.buffers[pixelBufferIndex]);
        }

        for (uint32_t const i : std::views::iota(0u, shaderEffectFile.effectData.constants.size()))
        {
            std::string namespaceName;
            std::string optionName;
            switch (shaderEffectFile.effectData.constants[i].constantType)
            {
                case rhi::fx::ShaderElementType::StorageBuffer:
                case rhi::fx::ShaderElementType::ConstantBuffer: {
                    namespaceName = shaderEffectFile.effectData.constants[i].constantName;
                    break;
                }
                default: {
                    optionName = shaderEffectFile.effectData.constants[i].constantName;
                    break;
                }
            }

            // Option is SamplerState, Texture or values. No need namespace there
            if (namespaceName.empty())
            {
                ShaderOption option = {
                    .constantIndex = i,
                    .elementType = shaderEffectFile.effectData.constants[i].constantType,
                    .offset = 0,
                    .size = rhi::fx::ShaderElementSize[shaderEffectFile.effectData.constants[i].constantType]};

                options.emplace(optionName, std::move(option));
            }
            else
            {
                uint32_t const structureIndex = shaderEffectFile.effectData.constants[i].structure;

                uint64_t offset = 0;
                for (auto const& structureElement : shaderEffectFile.effectData.structures[structureIndex].elements)
                {
                    ShaderOption option = {.constantIndex = i,
                                           .elementType = structureElement.elementType,
                                           .offset = offset,
                                           .size = rhi::fx::ShaderElementSize[structureElement.elementType]};

                    options.emplace(namespaceName + "." + structureElement.elementName, std::move(option));
                }
            }
        }
        return true;
    }
    */
} // namespace ionengine