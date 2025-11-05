// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "subpass.hpp"

namespace ionengine
{
    struct SubpassResourceInfo
    {
        core::ref_ptr<Subpass> subpass;
        int32_t colorIndex;
    };

    class GraphicsPipeline : public core::ref_counted_object
    {
      public:
        GraphicsPipeline(std::span<core::ref_ptr<Subpass>> const subpasses);

        auto bindInput(std::string_view const inputName, core::ref_ptr<rhi::Texture> inputTexture) -> void;

        auto execute(core::ref_ptr<rhi::RHI> rhi) -> void;

      private:
        std::vector<core::ref_ptr<Subpass>> _subpasses;
    };

    class GraphicsPipelineBuilder
    {
      public:
        GraphicsPipelineBuilder() = default;

        auto addSubpass(SubpassCreateInfo const& createInfo) -> GraphicsPipelineBuilder&;

        auto build() -> core::ref_ptr<GraphicsPipeline>;

      private:
        std::vector<core::ref_ptr<Subpass>> _subpasses;
        std::unordered_set<std::string> _subpassNames;
        std::unordered_map<std::string, std::vector<SubpassResourceInfo>> _resources;
    };
} // namespace ionengine