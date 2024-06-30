// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/fx/fx.hpp"
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::tools::shaderc
{
    class FXCompiler
    {
      public:
        FXCompiler();

        auto addIncludePath(std::filesystem::path const& includePath) -> void;

        auto compile(std::filesystem::path const& inputPath, std::filesystem::path const& outputPath,
                     std::string& errors) -> bool;

      private:
        winrt::com_ptr<IDxcCompiler3> compiler;
        std::vector<std::filesystem::path> includePaths;
        std::set<std::string> inputAssemblerNames;

        auto mergeShaderCode(std::string& shaderCode) -> bool;

        auto generateShaderConstants(std::string& shaderCode, std::vector<rhi::fx::ShaderConstantData>& constants,
                                     std::span<rhi::fx::ShaderStructureData const> const structures) -> bool;

        auto generateShaderStructures(std::string& shaderCode,
                                      std::vector<rhi::fx::ShaderStructureData>& structures) -> bool;

        auto generateShaderBindlessData(std::string& shaderCode,
                                        std::span<rhi::fx::ShaderConstantData const> const constants,
                                        std::span<rhi::fx::ShaderStructureData const> const structures,
                                        rhi::fx::ShaderTechniqueData const& technique) -> bool;

        auto generateShaderTechnique(std::string& shaderCode, rhi::fx::ShaderTechniqueData& technique) -> bool;

        auto compileShaderStage(std::string_view const shaderCode, std::string_view const entryPoint,
                                rhi::fx::ShaderStageType const stageType,
                                std::string& errors) -> std::optional<std::vector<uint8_t>>;
    };
} // namespace ionengine::tools::shaderc