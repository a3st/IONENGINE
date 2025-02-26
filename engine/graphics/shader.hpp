// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include "shadersys/fx.hpp"

namespace ionengine
{
    class Shader : public core::ref_counted_object
    {
      public:
        Shader(rhi::RHI& RHI, asset::ShaderFile const& shaderFile);

        auto getRasterizerStageInfo() const -> rhi::RasterizerStageInfo const&;

        auto getBindOffsets() const -> std::unordered_map<std::string, uint64_t> const&;

        auto getName() const -> std::string_view;

        auto getDescription() const -> std::string_view;

        auto getDomain() const -> std::string_view;

        auto getShader() -> core::ref_ptr<rhi::Shader>;

      private:
        core::ref_ptr<rhi::Shader> shader;
        rhi::RasterizerStageInfo rasterizerStageInfo;
        std::unordered_map<std::string, uint64_t> bindOffsets;
        std::string shaderName;
        std::string description;
        std::string domainName;
    };
} // namespace ionengine