// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include "shadersys/fx.hpp"

namespace ionengine
{
    class Shader : public core::ref_counted_object
    {
      public:
        struct Variant
        {
            core::ref_ptr<rhi::Shader> shaderProgram;
        };

        /*struct ShaderOption
        {
            uint32_t constantIndex;
            rhi::fx::ShaderElementType elementType;
            uint64_t offset;
            size_t size;
        };*/

        Shader(rhi::Device& device, shadersys::ShaderFile const& shaderFile);

        auto setActive(rhi::GraphicsContext& context, uint32_t const flags) -> void;

        auto getFlagsByName(std::string_view const permutationName) const -> uint32_t;

      private:
        std::unordered_map<std::string, uint32_t> permutationNames;
        std::unordered_map<uint32_t, Variant> shaderVariants;
        rhi::RasterizerStageInfo rasterizerStageInfo;
    };
} // namespace ionengine