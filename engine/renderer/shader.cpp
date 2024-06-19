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

    auto ShaderAsset::parseShaderEffectData(rhi::fx::ShaderEffectData const& shaderEffect) -> bool
    {
        std::string targetType;
        switch (shaderEffect.target)
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

        if (shaderEffect.technique.stages.find(rhi::fx::ShaderStageType::Compute) !=
            shaderEffect.technique.stages.end())
        {
            uint32_t const bufferIndex = shaderEffect.technique.stages.at(rhi::fx::ShaderStageType::Compute).buffer;
            shaderProgram = device->getDevice().createShader(shaderEffect.buffers[bufferIndex]);
        }
        else
        {
            uint32_t const vertexBufferIndex =
                shaderEffect.technique.stages.at(rhi::fx::ShaderStageType::Vertex).buffer;
            uint32_t const pixelBufferIndex = shaderEffect.technique.stages.at(rhi::fx::ShaderStageType::Pixel).buffer;

            rasterizerStage.fill_mode = rhi::FillMode::Solid;

            switch (shaderEffect.technique.cullSide)
            {
                case rhi::fx::ShaderCullSide::Back: {
                    rasterizerStage.cull_mode = rhi::CullMode::Back;
                    break;
                }
                case rhi::fx::ShaderCullSide::Front: {
                    rasterizerStage.cull_mode = rhi::CullMode::Front;
                    break;
                }
                case rhi::fx::ShaderCullSide::None: {
                    rasterizerStage.cull_mode = rhi::CullMode::None;
                    break;
                }
            }

            auto found = std::find_if(shaderEffect.structures.begin(), shaderEffect.structures.end(),
                                      [](auto const& element) { return element.name == "VS_INPUT"; });
            if (found == shaderEffect.structures.end())
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

            shaderProgram = device->getDevice().createShader(
                vertexDeclarations, shaderEffect.buffers[vertexBufferIndex], shaderEffect.buffers[pixelBufferIndex]);
        }

        for (uint32_t const i : std::views::iota(0u, shaderEffect.constants.size()))
        {
            std::string namespaceName;
            std::string optionName;
            switch (shaderEffect.constants[i].constantType)
            {
                case rhi::fx::ShaderElementType::StorageBuffer:
                case rhi::fx::ShaderElementType::ConstantBuffer: {
                    namespaceName = shaderEffect.constants[i].name;
                    break;
                }
                default: {
                    optionName = shaderEffect.constants[i].name;
                    break;
                }
            }

            // Option is SamplerState, Texture or values. No need namespace there
            if (namespaceName.empty())
            {
                ShaderOption option = {.constantIndex = i,
                                       .elementType = shaderEffect.constants[i].constantType,
                                       .offset = 0,
                                       .size = rhi::fx::ShaderElementSize[shaderEffect.constants[i].constantType]};

                options.emplace(optionName, std::move(option));
            }
            else
            {
                uint32_t const structureIndex = shaderEffect.constants[i].structure;

                uint64_t offset = 0;
                for (auto const& structureElement : shaderEffect.structures[structureIndex].elements)
                {
                    ShaderOption option = {.constantIndex = i,
                                           .elementType = structureElement.elementType,
                                           .offset = offset,
                                           .size = rhi::fx::ShaderElementSize[structureElement.elementType]};

                    options.emplace(namespaceName + "." + structureElement.name, std::move(option));
                }
            }
        }
        return true;
    }

    auto ShaderAsset::loadFromFile(std::filesystem::path const& filePath) -> bool
    {
        auto result = core::loadFromFile<rhi::fx::ShaderEffectData>(filePath);
        if (!result.has_value())
        {
            return false;
        }

        rhi::fx::ShaderEffectData shaderEffect = std::move(result.value());
        return this->parseShaderEffectData(shaderEffect);
    }

    auto ShaderAsset::loadFromBytes(std::span<uint8_t const> const dataBytes) -> bool
    {
        auto result = core::loadFromBytes<rhi::fx::ShaderEffectData>(dataBytes);
        if (!result.has_value())
        {
            return false;
        }

        rhi::fx::ShaderEffectData shaderEffect = std::move(result.value());
        return this->parseShaderEffectData(shaderEffect);
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