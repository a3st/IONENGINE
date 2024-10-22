// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include "shadersys/fx.hpp"

namespace ionengine
{
    class Shader : public core::ref_counted_object
    {
      public:
        Shader(rhi::Device& device, shadersys::ShaderFile const& shaderFile);

        auto getPermutationNames() const -> std::unordered_map<std::string, uint32_t> const&;

        auto getShader(uint32_t const flags) -> core::ref_ptr<rhi::Shader>;

        auto getStructureNames() const -> std::unordered_map<std::string, shadersys::fx::StructureData> const&;

        auto getRasterizerStageInfo() const -> rhi::RasterizerStageInfo const&;

      private:
        std::unordered_map<std::string, uint32_t> permutationNames;
        std::unordered_map<uint32_t, core::ref_ptr<rhi::Shader>> shaders;
        std::unordered_map<std::string, shadersys::fx::StructureData> structureNames;
        rhi::RasterizerStageInfo rasterizerStageInfo;
    };
} // namespace ionengine