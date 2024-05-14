// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/fx/fxsl.hpp"

namespace ionengine::tools::shaderc
{
    class ShaderCompiler;

    class FXCompiler
    {
      public:
        FXCompiler();

        auto addIncludePath(std::filesystem::path const& includePath) -> void;

        auto compile(ShaderCompiler& compiler, std::filesystem::path const& inputPath,
                     std::filesystem::path const& outputPath, std::string& errors) -> bool;

      private:
        std::vector<std::filesystem::path> includePaths;
        std::set<std::string> inputAssemblerNames;

        auto mergeShaderCode(std::string& shaderCode) -> bool;

        auto generateShaderConstants(std::string& shaderCode, std::vector<rhi::fx::ShaderConstantData>& constants,
                                     std::span<rhi::fx::ShaderStructureData const> const structures) -> bool;

        auto generateShaderStructures(std::string& shaderCode,
                                      std::vector<rhi::fx::ShaderStructureData>& structures) -> bool;

        auto generateBindlessData(std::string& shaderCode, std::span<rhi::fx::ShaderConstantData const> const constants,
                                  std::span<rhi::fx::ShaderStructureData const> const structures,
                                  rhi::fx::ShaderTechniqueData const& technique) -> bool;

        auto generateShaderTechnique(std::string& shaderCode, rhi::fx::ShaderTechniqueData& technique) -> bool;
    };
} // namespace ionengine::tools::shaderc