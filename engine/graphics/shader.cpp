// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "shader.hpp"
#include "precompiled.h"

namespace ionengine
{
    auto FXVertexFormat_to_RHIVertexFormat(asset::fx::VertexFormat const format) -> rhi::VertexFormat
    {
        switch (format)
        {
            case asset::fx::VertexFormat::RGBA32_FLOAT:
                return rhi::VertexFormat::RGBA32_FLOAT;
            case asset::fx::VertexFormat::RGBA32_SINT:
                return rhi::VertexFormat::RGBA32_SINT;
            case asset::fx::VertexFormat::RGBA32_UINT:
                return rhi::VertexFormat::RGBA32_UINT;
            case asset::fx::VertexFormat::RGB32_FLOAT:
                return rhi::VertexFormat::RGB32_FLOAT;
            case asset::fx::VertexFormat::RGB32_SINT:
                return rhi::VertexFormat::RGB32_SINT;
            case asset::fx::VertexFormat::RGB32_UINT:
                return rhi::VertexFormat::RGB32_UINT;
            case asset::fx::VertexFormat::RG32_FLOAT:
                return rhi::VertexFormat::RG32_FLOAT;
            case asset::fx::VertexFormat::RG32_SINT:
                return rhi::VertexFormat::RG32_SINT;
            case asset::fx::VertexFormat::RG32_UINT:
                return rhi::VertexFormat::RG32_UINT;
            case asset::fx::VertexFormat::R32_FLOAT:
                return rhi::VertexFormat::R32_FLOAT;
            case asset::fx::VertexFormat::R32_SINT:
                return rhi::VertexFormat::R32_SINT;
            case asset::fx::VertexFormat::R32_UINT:
                return rhi::VertexFormat::R32_UINT;
            default:
                throw std::invalid_argument("unknown VertexFormat for passed argument");
        }
    }

    auto FXFillMode_to_RHIFillMode(asset::fx::FillMode const fillMode) -> rhi::FillMode
    {
        switch (fillMode)
        {
            case asset::fx::FillMode::Solid:
                return rhi::FillMode::Solid;
            case asset::fx::FillMode::Wireframe:
                return rhi::FillMode::Wireframe;
            default:
                throw std::invalid_argument("unknown FillMode for passed argument");
        }
    }

    auto FXCullMode_to_RHICullMode(asset::fx::CullMode const cullMode) -> rhi::CullMode
    {
        switch (cullMode)
        {
            case asset::fx::CullMode::Front:
                return rhi::CullMode::Front;
            case asset::fx::CullMode::Back:
                return rhi::CullMode::Back;
            case asset::fx::CullMode::None:
                return rhi::CullMode::None;
            default:
                throw std::invalid_argument("unknown CullMode for passed argument");
        }
    }

    Shader::Shader(rhi::RHI& RHI, asset::ShaderFile const& shaderFile)
    {
        std::string shaderFormat;
        switch (shaderFile.shaderFormat)
        {
            case asset::fx::ShaderFormat::DXIL: {
                shaderFormat = "D3D12";
                break;
            }
            case asset::fx::ShaderFormat::SPIRV: {
                shaderFormat = "Vulkan";
                break;
            }
        }

        if (shaderFormat.compare(RHI.getName()) != 0)
        {
            throw std::runtime_error("shader has a different format (for Vulkan use SPIR-V instead DXIL)");
        }

        rhi::ShaderCreateInfo shaderCreateInfo{};
        std::vector<rhi::VertexDeclarationInfo> vertexDeclarations;

        for (auto const& [stageType, stageData] : shaderFile.shaderData.stages)
        {
            auto const& bufferData = shaderFile.shaderData.buffers[stageData.buffer];

            rhi::ShaderStageCreateInfo const stageCreateInfo{
                .entryPoint = stageData.entryPoint,
                .shaderCode = {shaderFile.blob.data() + bufferData.offset, bufferData.size}};

            if (stageType == asset::fx::StageType::Vertex)
            {
                auto const& vertexLayout = stageData.vertexLayout.value();
                for (auto& element : vertexLayout.elements)
                {
                    rhi::VertexDeclarationInfo const vertexDeclaration{
                        .semantic = element.semantic, .format = FXVertexFormat_to_RHIVertexFormat(element.format)};
                    vertexDeclarations.emplace_back(std::move(vertexDeclaration));
                }

                shaderCreateInfo.graphics.vertexDeclarations = vertexDeclarations;
                shaderCreateInfo.graphics.vertexStage = std::move(stageCreateInfo);
            }
            else if (stageType == asset::fx::StageType::Pixel)
            {
                auto const& outputData = stageData.output.value();
                rasterizerStageInfo.fillMode = FXFillMode_to_RHIFillMode(outputData.fillMode);
                rasterizerStageInfo.cullMode = FXCullMode_to_RHICullMode(outputData.cullMode);

                shaderCreateInfo.shaderType = rhi::ShaderType::Graphics;
                shaderCreateInfo.graphics.pixelStage = std::move(stageCreateInfo);
            }
            else if (stageType == asset::fx::StageType::Compute)
            {
                shaderCreateInfo.shaderType = rhi::ShaderType::Compute;
                shaderCreateInfo.compute = std::move(stageCreateInfo);
            }
        }

        shader = RHI.createShader(shaderCreateInfo);

        for (auto const& structure : shaderFile.shaderData.structures)
        {
            uint64_t offset = 0;
            for (auto const& element : structure.elements)
            {
                std::string const bindName = structure.name + "." + element.name;
                bindOffsets[bindName] = offset;
                offset += asset::fx::sizeof_ElementType(element.type);
            }
        }
    }

    auto Shader::getRasterizerStageInfo() const -> rhi::RasterizerStageInfo const&
    {
        return rasterizerStageInfo;
    }

    auto Shader::getBindOffsets() const -> std::unordered_map<std::string, uint64_t> const&
    {
        return bindOffsets;
    }

    auto Shader::getName() const -> std::string_view
    {
        return shaderName;
    }

    auto Shader::getDescription() const -> std::string_view
    {
        return description;
    }

    auto Shader::getDomain() const -> std::string_view
    {
        return domainName;
    }

    auto Shader::getShader() -> core::ref_ptr<rhi::Shader>
    {
        return shader;
    }
} // namespace ionengine