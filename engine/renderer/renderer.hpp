// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/matrix.hpp"
#include "renderable.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace ionengine
{
    class LinkedDevice;

    class Renderer
    {
      public:
        Renderer(LinkedDevice& device);

        auto createTextureAsset(uint32_t const width, uint32_t const height,
                                TextureUsage const usage) -> core::ref_ptr<TextureAsset>;

        auto createShaderAsset() -> core::ref_ptr<ShaderAsset>;

        auto drawQuad() -> void;

        auto beginDraw(std::span<core::ref_ptr<rhi::Texture>> colors, core::ref_ptr<rhi::Texture> depthStencil,
                       std::optional<math::Color> const clearColor, std::optional<float> const clearDepth,
                       std::optional<uint8_t> const clearStencil) -> void;

        auto endDraw() -> void;

        auto resize(uint32_t const width, uint32_t const height) -> void;

        auto render(std::span<RenderableData> const renderables) -> void;

        core::ref_ptr<rhi::Texture> swapchainTexture;

      private:
        LinkedDevice* device;
        std::stack<core::ref_ptr<rhi::Texture>> colorBarriers;
        uint32_t rendererWidth;
        uint32_t rendererHeight;
        core::ref_ptr<rhi::Shader> currentShader;
    };
} // namespace ionengine