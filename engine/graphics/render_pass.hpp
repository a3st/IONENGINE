// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "render_queue.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class BufferAllocator;
    class UploadManager;

    enum class RenderGroup
    {
        Opaque,
        Translucent,
        UI
    };

    struct RenderableData
    {
        std::unordered_map<RenderGroup, RenderQueue> renderGroups;
    };

    struct RenderPassInputInfo
    {
        std::string bindingName;
        core::ref_ptr<rhi::Texture> texture;
    };

    struct RenderContext
    {
        rhi::GraphicsContext* graphics;
        UploadManager* uploadManager;
        BufferAllocator* constBufferAllocator;
        core::ref_ptr<rhi::Buffer> samplerDataBuffer;
    };

    class RenderPass
    {
      public:
        RenderPass(std::string_view const passName);

        virtual ~RenderPass() = default;

        virtual auto execute(RenderContext const& context, RenderableData const& renderableData) -> void = 0;

        auto getColors() const -> std::vector<rhi::RenderPassColorInfo> const&;

        auto getHash() const -> uint64_t;

        auto getInputs() const -> std::vector<RenderPassInputInfo> const&;

      protected:
        auto initializeRenderPass() -> void;

        std::vector<rhi::RenderPassColorInfo> colors;
        std::vector<RenderPassInputInfo> inputs;
        std::optional<rhi::RenderPassDepthStencilInfo> depthStencil;

      private:
        std::string passName;
        uint64_t hash;
    };
} // namespace ionengine