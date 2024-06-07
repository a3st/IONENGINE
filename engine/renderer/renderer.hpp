// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/matrix.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace ionengine
{
    class LinkedDevice;

    class Renderer
    {
      public:
        Renderer(LinkedDevice& device);

        auto registerShader(std::string_view const shaderName, core::ref_ptr<ShaderAsset> shaderAsset) -> void;

        auto setShader(std::string_view const shaderName) -> void;

        auto drawQuad(math::Matrixf const& viewProjection) -> void;

        auto beginDraw(std::span<core::ref_ptr<rhi::Texture>> colors, core::ref_ptr<rhi::Texture> depthStencil,
                       std::optional<math::Color> const clearColor, std::optional<float> const clearDepth,
                       std::optional<uint8_t> const clearStencil) -> void;

        auto endDraw() -> void;

        core::ref_ptr<rhi::Texture> swapchainTexture;

      private:
        LinkedDevice* device;
        std::stack<core::ref_ptr<rhi::Texture>> colorBarriers;
        std::unordered_map<std::string, core::ref_ptr<ShaderAsset>> registeredShaders;
    };
} // namespace ionengine