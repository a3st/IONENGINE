// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader.hpp"
#include "linked_device.hpp"
#include "precompiled.h"

namespace ionengine
{
    auto fxShaderElementToRHI(rhi::fx::ShaderElementType const format) -> rhi::VertexFormat
    {
        switch (format)
        {
            case rhi::fx::ShaderElementType::Float: {
                return rhi::VertexFormat::Float;
            }
            case rhi::fx::ShaderElementType::Float2: {
                return rhi::VertexFormat::Float2;
            }
            case rhi::fx::ShaderElementType::Float3: {
                return rhi::VertexFormat::Float3;
            }
            case rhi::fx::ShaderElementType::Float4: {
                return rhi::VertexFormat::Float4;
            }
            case rhi::fx::ShaderElementType::Float4x4: {
                return rhi::VertexFormat::Float4x4;
            }
            case rhi::fx::ShaderElementType::Bool: {
                return rhi::VertexFormat::Bool;
            }
            case rhi::fx::ShaderElementType::Uint: {
                return rhi::VertexFormat::Uint;
            }
            default: {
                throw std::invalid_argument("Passed invalid argument into function");
            }
        }
    }

    auto isSemanticVertexDeclaration(std::string_view const semantic) -> bool
    {
        if (semantic.contains("POSITION"))
        {
            return true;
        }
        else if (semantic.contains("NORMAL"))
        {
            return true;
        }
        else if (semantic.contains("TEXCOORD"))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    ShaderAsset::ShaderAsset(LinkedDevice& device) : device(&device)
    {
    }

    auto ShaderAsset::getShader() const -> core::ref_ptr<rhi::Shader>
    {
        return shaderProgram;
    }

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
                std::cerr << "[Renderer] ShaderAsset {} hasn't input assembler and cannot be used as a vertex shader"
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
                    .format = fxShaderElementToRHI(structureElement.elementType)};

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

    auto ShaderAsset::loadFromFile(std::filesystem::path const& filePath) -> bool
    {
        auto result = core::loadFromFile<rhi::fx::ShaderEffectFile>(filePath);
        if (!result.has_value())
        {
            return false;
        }

        rhi::fx::ShaderEffectFile shaderEffectFile = std::move(result.value());
        return this->parseShaderEffectFile(shaderEffectFile);
    }

    auto ShaderAsset::loadFromBytes(std::span<uint8_t const> const dataBytes) -> bool
    {
        auto result = core::loadFromBytes<rhi::fx::ShaderEffectFile>(dataBytes);
        if (!result.has_value())
        {
            return false;
        }

        rhi::fx::ShaderEffectFile shaderEffectFile = std::move(result.value());
        return this->parseShaderEffectFile(shaderEffectFile);
    }

    auto ShaderAsset::getOptions() const -> std::unordered_map<std::string, ShaderOption>
    {
        return options;
    }

    auto ShaderAsset::getRasterizerStage() const -> rhi::RasterizerStageInfo const&
    {
        return rasterizerStage;
    }
} // namespace ionengine