// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "subpass.hpp"
#include "precompiled.h"

namespace ionengine
{
    Subpass::Subpass(SubpassCreateInfo const& createInfo)
        : _inputs(createInfo.inputs), _colors(createInfo.colors), _depthStencil(createInfo.depthStencil)
    {
        for (auto const& color : createInfo.colors)
        {
            rhi::RenderPassColorInfo renderPassColorInfo{
                .loadOp = color.loadOp, .storeOp = color.storeOp, .clearColor = color.clearColor};

            _rhiColors.emplace_back(std::move(renderPassColorInfo));
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

            _rhiDepthStencil.emplace(std::move(renderPassDepthStencilInfo));
        }
    }

    auto Subpass::beginPass(rhi::GraphicsContext* context, std::span<rhi::Texture* const> const colorTextures,
                            rhi::Texture* const depthStencilTexture) -> void
    {
        assert(colorTextures.size() == _colors.size() && "colorTextures should be equal to _colors size");

        for (uint32_t const i : std::views::iota(0u, _colors.size()))
        {
            _rhiColors[i].texture = colorTextures[i];
        }

        if (_depthStencil.has_value())
        {
            _rhiDepthStencil.value().texture = depthStencilTexture;
        }

        context->beginRenderPass(_rhiColors, _rhiDepthStencil);
    }

    auto Subpass::endPass(rhi::GraphicsContext* context) -> void
    {
        context->endRenderPass();
    }

    auto Subpass::getInputs() const -> std::span<SubpassInputInfo const>
    {
        return _inputs;
    }

    auto Subpass::getColors() const -> std::span<SubpassColorInfo const>
    {
        return _colors;
    }

    auto Subpass::getDepthStencil() const -> std::optional<SubpassDepthStencilInfo> const&
    {
        return _depthStencil;
    }
} // namespace ionengine