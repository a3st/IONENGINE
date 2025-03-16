// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "render_queue.hpp"
#include "rhi/rhi.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "upload_manager.hpp"

namespace ionengine
{
    struct RenderPassInputInfo
    {
        std::string bindingName;
        core::ref_ptr<rhi::Texture> texture;
    };

    struct RenderContext
    {
        rhi::GraphicsContext* graphics;
        internal::UploadManager* uploadManager;
        BufferAllocator* constBufferAllocator;
    };

    class RenderPass
    {
      public:
        RenderPass(std::string_view const debugName);

        virtual auto execute(RenderContext const& context, RenderableData const& renderableData) -> void = 0;

        auto getColors() const -> std::span<rhi::RenderPassColorInfo const> const;

        auto getHash() const -> uint64_t;

        auto getShader() -> core::ref_ptr<Shader>;

        auto getInputs() const -> std::span<RenderPassInputInfo const> const;

      protected:
        auto initializeRenderPass() -> void;

        std::vector<rhi::RenderPassColorInfo> colors;
        std::vector<RenderPassInputInfo> inputs;
        std::optional<rhi::RenderPassDepthStencilInfo> depthStencil;
        core::ref_ptr<Shader> shader;

      private:
        std::string debugName;
        uint64_t hash;
    };
} // namespace ionengine