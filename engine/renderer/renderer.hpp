// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/handle.hpp"
#include "model.hpp"
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

    struct DrawResults
    {
        std::vector<core::ref_ptr<Surface>> visibleSurfaces;
    };

    struct RenderingData
    {
        DrawResults drawResults;
        core::ref_ptr<rhi::Texture> backBuffer;
    };

    class RenderPass : public core::ref_counted_object
    {
      public:
        virtual ~RenderPass() = default;

        virtual auto render(RenderingData const& renderingData) -> void = 0;

        rhi::GraphicsContext* graphicsContext;
        rhi::CopyContext* copyContext;
    };

    class Renderer : public core::ref_counted_object
    {
      public:
        Renderer(rhi::RHICreateInfo const& createInfo, std::vector<core::ref_ptr<RenderPass>> const& renderPasses);

        auto createShader(shadersys::ShaderFile const& shaderFile) -> core::ref_ptr<Shader>;

        auto createTexture() -> core::ref_ptr<Texture>;

        auto render(RenderingData& renderingData) -> void;

        auto resize(uint32_t const width, uint32_t const height) -> void;

        /*

        */

      private:
        core::ref_ptr<rhi::Device> device;

        core::ref_ptr<rhi::GraphicsContext> graphicsContext;
        core::ref_ptr<rhi::CopyContext> copyContext;

        std::vector<core::ref_ptr<RenderPass>> renderPasses;
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

        auto build(rhi::RHICreateInfo const& createInfo) -> core::ref_ptr<Renderer>;

      private:
        std::vector<core::ref_ptr<RenderPass>> renderPasses;
    };
} // namespace ionengine