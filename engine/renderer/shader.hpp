// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include "shadersys/fx.hpp"

namespace ionengine
{
    class ShaderVariant : public core::ref_counted_object
    {
      public:
        ShaderVariant(core::ref_ptr<rhi::Shader> shaderProgram, rhi::RasterizerStageInfo& rasterizerStageInfo);

        auto setActive(rhi::GraphicsContext& context) -> void;

      private:
        core::ref_ptr<rhi::Shader> shaderProgram;
        rhi::RasterizerStageInfo* rasterizerStageInfo;
    };

    class Shader : public core::ref_counted_object
    {
      public:
        Shader(rhi::Device& device, shadersys::ShaderFile const& shaderFile);

        auto getFlagsByName(std::string_view const permutationName) const -> uint32_t;

        auto getVariant(uint32_t const flags) -> core::ref_ptr<ShaderVariant>;

        auto getMaterialData() -> std::optional<shadersys::fx::StructureData>;

      private:
        std::unordered_map<std::string, uint32_t> permutationNames;
        std::unordered_map<uint32_t, core::ref_ptr<ShaderVariant>> shaderVariants;
        rhi::RasterizerStageInfo rasterizerStageInfo;
        std::optional<shadersys::fx::StructureData> materialData;
    };
} // namespace ionengine