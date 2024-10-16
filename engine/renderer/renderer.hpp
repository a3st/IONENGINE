// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer_pool.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "math/matrix.hpp"
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

        /*auto createTexture(rhi::TextureFormat const format,
                           rhi::TextureUsage const usage) -> core::handle<RenderPassTexture>;*/
    };

    struct ObjectData
    {
        core::ref_ptr<Surface> surface;
        core::ref_ptr<Shader> shader;
        core::ref_ptr<rhi::Buffer> materialBuffer;
    };

    struct DrawResults
    {
        std::vector<ObjectData> visibleObjects;
        std::vector<shadersys::common::LightingData> visibleLights;
    };

    struct RenderingData
    {
        DrawResults drawResults;
        core::weak_ptr<rhi::Texture> backBuffer;
    };

    class RenderPass : public core::ref_counted_object
    {
      public:
        virtual ~RenderPass() = default;

        virtual auto render(RenderingData const& renderingData) -> void = 0;

        rhi::GraphicsContext* graphicsContext;
        rhi::CopyContext* copyContext;
        ConstantBufferPool* constantBufferPool;
    };

    class Renderer : public core::ref_counted_object
    {
      public:
        Renderer(rhi::RHICreateInfo const& createInfo, std::vector<core::ref_ptr<RenderPass>> const& renderPasses);

        auto createShader(shadersys::ShaderFile const& shaderFile) -> core::ref_ptr<Shader>;

        auto createTexture() -> core::ref_ptr<Texture>;

        auto createModel(ModelFile const& modelFile) -> core::ref_ptr<Model>;

        auto createMaterial(Shader& shader) -> core::ref_ptr<Material>;

        auto render(RenderingData& renderingData) -> void;

        auto resize(uint32_t const width, uint32_t const height) -> void;

      private:
        core::ref_ptr<rhi::Device> device;

        core::ref_ptr<rhi::GraphicsContext> graphicsContext;
        core::ref_ptr<rhi::CopyContext> copyContext;

        core::ref_ptr<ConstantBufferPool> constantBufferPool;

        std::vector<core::ref_ptr<RenderPass>> renderPasses;

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