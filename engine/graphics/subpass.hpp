// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/color.hpp"
#include "core/ref_ptr.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    struct SubpassColorInfo
    {
        std::string name;
        rhi::RenderPassLoadOp loadOp;
        rhi::RenderPassStoreOp storeOp;
        core::Color clearColor;
    };

    struct SubpassDepthStencilInfo
    {
        std::string name;
        rhi::RenderPassLoadOp depthLoadOp;
        rhi::RenderPassStoreOp depthStoreOp;
        rhi::RenderPassLoadOp stencilLoadOp;
        rhi::RenderPassStoreOp stencilStoreOp;
        float clearDepth;
        uint8_t clearStencil;
    };

    struct SubpassInputInfo
    {
        std::string name;
    };

    struct SubpassCreateInfo
    {
        std::string name;
        std::vector<SubpassInputInfo> inputs;
        std::vector<SubpassColorInfo> colors;
        std::optional<SubpassDepthStencilInfo> depthStencil;
    };

    class Subpass : public core::ref_counted_object
    {
      public:
        Subpass(SubpassCreateInfo const& createInfo);

        auto beginPass(rhi::GraphicsContext& context) -> void;

        auto endPass(rhi::GraphicsContext& context) -> void;

        auto getColors() const -> std::span<rhi::RenderPassColorInfo const>;

        auto getDepthStencil() const -> std::optional<rhi::RenderPassDepthStencilInfo> const&;

      private:
        std::vector<rhi::RenderPassColorInfo> _colors;
        std::optional<rhi::RenderPassDepthStencilInfo> _depthStencil;
    };
} // namespace ionengine