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

        for (auto const& [permutationName, flags] : shaderFile.shaderData.permutations)
        {
            permutationNames[permutationName] = flags;
        }

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

        for (auto const& [flags, shaderData] : shaderFile.shaderData.shaders)
        {
            if (flags == permutationNames["BASE"])
            {
                for (auto const& element : shaderData.structures)
                {
                    structureNames[element.name] = element;
                }
            }

            rhi::ShaderCreateInfo shaderCreateInfo{.pipelineType = rhi::PipelineType::Graphics};

            for (auto const& [stageType, stageData] : shaderData.stages)
            {
                uint32_t const bufferIndex = stageData.buffer;
                auto const& bufferData = shaderFile.shaderData.buffers[bufferIndex];

                if (stageType == shadersys::fx::StageType::Compute)
                {
                    shaderCreateInfo.pipelineType = rhi::PipelineType::Compute;

                    rhi::ShaderStageCreateInfo stageCreateInfo{
                        .entryPoint = stageData.entryPoint,
                        .shaderCode = {shaderFile.blob.data() + bufferData.offset, bufferData.size}};
                    shaderCreateInfo.compute = std::move(stageCreateInfo);
                }
                else
                {
                    rhi::ShaderStageCreateInfo stageCreateInfo{
                        .entryPoint = stageData.entryPoint,
                        .shaderCode = {shaderFile.blob.data() + bufferData.offset, bufferData.size}};
                    switch (stageType)
                    {
                        case shadersys::fx::StageType::Vertex: {
                            for (auto const& inputElement : stageData.vertexLayout.elements)
                            {
                                rhi::VertexDeclarationInfo vertexDeclarationInfo{
                                    .semantic = inputElement.semantic,
                                    .format = FXVertexFormat_to_RHIVertexFormat(inputElement.format)};

                                shaderCreateInfo.graphics.vertexDeclarations.emplace_back(
                                    std::move(vertexDeclarationInfo));
                            }

                            shaderCreateInfo.graphics.vertexStage = std::move(stageCreateInfo);
                            break;
                        }
                        case shadersys::fx::StageType::Pixel: {
                            shaderCreateInfo.graphics.pixelStage = std::move(stageCreateInfo);
                            break;
                        }
                    }
                }
            }

            shaders[flags] = device.createShader(shaderCreateInfo);
        }
    }

    auto Shader::getPermutationNames() const -> std::unordered_map<std::string, uint32_t> const&
    {
        return permutationNames;
    }

    auto Shader::getShader(uint32_t const flags) -> core::ref_ptr<rhi::Shader>
    {
        auto result = shaders.find(flags);
        if (result == shaders.end())
        {
            throw core::runtime_error("An error occurred while getting shader by flags");
        }
        return result->second;
    }

    auto Shader::getStructureNames() const -> std::unordered_map<std::string, shadersys::fx::StructureData> const&
    {
        return structureNames;
    }

    auto Shader::getRasterizerStageInfo() const -> rhi::RasterizerStageInfo const&
    {
        return rasterizerStageInfo;
    }
} // namespace ionengine