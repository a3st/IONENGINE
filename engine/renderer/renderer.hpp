// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace ionengine
{
    class RenderPass : public core::ref_counted_object
    {
      public:
        virtual auto render() -> void = 0;
    };

    class RendererBuilder
    {
      public:
        RendererBuilder();

        template <typename Type = RenderPass>
        auto addPass();

      private:
        std::vector<core::ref_ptr<RenderPass>> renderPasses;
    };

    class Renderer : core::ref_counted_object
    {
      public:
        Renderer(rhi::Device& device);

        auto createShader(shadersys::fx::ShaderEffectFile const& shaderEffect) -> core::ref_ptr<Shader>;

        auto createTexture() -> core::ref_ptr<Texture>;

        auto render() -> void;

        /*

        */

      private:
        rhi::Device* device;

        std::unordered_map<std::string, core::ref_ptr<Shader>> shaders;
    };
} // namespace ionengine