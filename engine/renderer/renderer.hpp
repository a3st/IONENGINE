// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/handle.hpp"
#include "rhi/rhi.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace ionengine
{
    struct RenderPassTexture
    {
    };

    class RenderPassCreateInfo
    {
      public:
        RenderPassCreateInfo() = default;

        auto createTexture(rhi::TextureFormat const format,
                           rhi::TextureUsage const usage) -> core::handle<RenderPassTexture>;
    };

    class RenderPassStorage : public core::ref_counted_object
    {
      public:
        auto getTexture(core::handle<RenderPassTexture> const handle) -> core::ref_ptr<rhi::Texture>;

        core::ref_ptr<rhi::Texture> backBuffer;
    };

    class RenderPass : public core::ref_counted_object
    {
      public:
        virtual ~RenderPass() = default;

        virtual auto render() -> void = 0;

        rhi::GraphicsContext* graphicsContext;
        rhi::CopyContext* copyContext;
        RenderPassStorage* passStorage;
    };

    class Renderer : public core::ref_counted_object
    {
      public:
        Renderer(rhi::Device& device, std::vector<core::ref_ptr<RenderPass>> const& renderPasses,
                 core::ref_ptr<RenderPassStorage> passStorage);

        auto createShader(shadersys::fx::ShaderEffectFile const& shaderEffect) -> core::ref_ptr<Shader>;

        auto createTexture() -> core::ref_ptr<Texture>;

        auto render() -> void;

        auto resize(uint32_t const width, uint32_t const height) -> void;

        /*

        */

      private:
        rhi::Device* device;

        core::ref_ptr<rhi::GraphicsContext> graphicsContext;
        core::ref_ptr<rhi::CopyContext> copyContext;

        std::vector<core::ref_ptr<RenderPass>> renderPasses;
        core::ref_ptr<RenderPassStorage> passStorage;
        std::unordered_map<std::string, core::ref_ptr<Shader>> shaders;

        uint32_t outputWidth;
        uint32_t outputHeight;
    };

    class RendererBuilder
    {
      public:
        RendererBuilder() = default;

        template <typename Type = RenderPass>
        auto addPass() -> RendererBuilder&
        {
            RenderPassCreateInfo passCreateInfo;
            auto renderPass = core::make_ref<Type>(passCreateInfo);
            renderPasses.emplace_back(std::move(renderPass));
            return *this;
        }

        auto build(rhi::Device& device) -> core::ref_ptr<Renderer>;

      private:
        std::vector<core::ref_ptr<RenderPass>> renderPasses;
        core::ref_ptr<RenderPassStorage> passStorage;
    };
} // namespace ionengine