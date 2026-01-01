// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "subpass.hpp"
#include "precompiled.h"

namespace ionengine
{
    Subpass::Subpass(SubpassCreateInfo const& createInfo)
    {
        for (auto const& color : createInfo.colors)
        {
            rhi::RenderPassColorInfo renderPassColorInfo{
                .loadOp = color.loadOp, .storeOp = color.storeOp, .clearColor = color.clearColor};

            _colors.emplace_back(std::move(renderPassColorInfo));
        }

        if (createInfo.depthStencil.has_value())
        {
            auto depthStencilValue = createInfo.depthStencil.value();

            rhi::RenderPassDepthStencilInfo renderPassDepthStencilInfo{.depthLoadOp = depthStencilValue.depthLoadOp,
                                                                       .depthStoreOp = depthStencilValue.depthStoreOp,
                                                                       .stencilLoadOp = depthStencilValue.stencilLoadOp,
                                                                       .stencilStoreOp =
                                                                           depthStencilValue.stencilStoreOp,
                                                                       .clearDepth = depthStencilValue.clearDepth,
                                                                       .clearStencil = depthStencilValue.clearStencil};

            _depthStencil.emplace(std::move(renderPassDepthStencilInfo));
        }
    }

    auto Subpass::beginPass(rhi::GraphicsContext& context) -> void
    {
        context.beginRenderPass(_colors, _depthStencil);
    }

    auto Subpass::endPass(rhi::GraphicsContext& context) -> void
    {
        context.endRenderPass();
    }

    auto Subpass::getColors() const -> std::span<rhi::RenderPassColorInfo const>
    {
        return _colors;
    }

    auto Subpass::getDepthStencil() const -> std::optional<rhi::RenderPassDepthStencilInfo> const&
    {
        return _depthStencil;
    }
} // namespace ionengine