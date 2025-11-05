// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics_pipeline.hpp"
#include "precompiled.h"

namespace ionengine
{
    GraphicsPipeline::GraphicsPipeline(std::span<core::ref_ptr<Subpass>> const subpasses)
    {
    }

    auto GraphicsPipeline::bindInput(std::string_view const inputName, core::ref_ptr<rhi::Texture> inputTexture) -> void
    {
    }

    auto GraphicsPipelineBuilder::addSubpass(SubpassCreateInfo const& createInfo) -> GraphicsPipelineBuilder&
    {
        if (_subpassNames.count(createInfo.name))
        {
            return *this;
        }

        auto subpass = core::make_ref<Subpass>(createInfo);

        for (uint32_t const i : std::views::iota(0u, createInfo.colors.size()))
        {
            SubpassResourceInfo resourceInfo{.subpass = subpass, .colorIndex = static_cast<int32_t>(i)};

            _resources.try_emplace(createInfo.colors[i].name);
            _resources[createInfo.colors[i].name].emplace_back(std::move(resourceInfo));
        }

        if (createInfo.depthStencil.has_value())
        {
            auto depthStencilValue = createInfo.depthStencil.value();

            SubpassResourceInfo resourceInfo{.subpass = subpass, .colorIndex = -1};

            _resources.try_emplace(depthStencilValue.name);
            _resources[depthStencilValue.name].emplace_back(std::move(resourceInfo));
        }

        for (auto const& input : createInfo.inputs)
        {
            SubpassResourceInfo resourceInfo{.subpass = subpass, .colorIndex = -2};

            _resources.try_emplace(input.name);
            _resources[input.name].emplace_back(std::move(resourceInfo));
        }

        _subpasses.emplace_back(subpass);
        return *this;
    }

    auto GraphicsPipelineBuilder::build() -> core::ref_ptr<GraphicsPipeline>
    {
        return nullptr;
    }
} // namespace ionengine