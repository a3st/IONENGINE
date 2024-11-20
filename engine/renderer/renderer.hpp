// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer_pool.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "math/matrix.hpp"
#include "model.hpp"
#include "queue.hpp"
#include "texture.hpp"

namespace ionengine
{
    struct RenderingData
    {
        core::ref_ptr<rhi::Texture> renderTarget;
    };

    /*class RenderPass : public core::ref_counted_object
    {
      public:
        virtual ~RenderPass() = default;

        auto beginPass() -> void;

        virtual auto render(RenderingData const& renderingData, rhi::GraphicsContext& graphicsContext) -> void = 0;

        auto endPass() -> void;

        auto drawSurface(rhi::GraphicsContext& graphicsContext, core::ref_ptr<Material> material) -> void;

        auto drawQuad(rhi::GraphicsContext& graphicsContext, core::ref_ptr<Material> material) -> void;

        auto readTexture(core::ref_ptr<rhi::Texture> texture) -> void;

        rhi::GraphicsContext* graphicsContext;
        rhi::CopyContext* copyContext;
        ConstantBufferPool* constantBufferPool;
        bool isInitialized;

        core::ref_ptr<Material> material;

        std::vector<core::ref_ptr<rhi::Texture>> outputs;
    };*/

    class Renderer : public core::ref_counted_object
    {
      public:
        Renderer(rhi::RHICreateInfo const& createInfo);

        auto createShader(asset::ShaderFile const& shaderFile) -> core::ref_ptr<Shader>;

        auto createModel(asset::ModelFile const& modelFile) -> core::ref_ptr<Model>;

        auto createMaterial(core::ref_ptr<Shader> shader) -> core::ref_ptr<Material>;

        auto createTexture() -> core::ref_ptr<Texture>;

        auto render() -> void;

        auto resize(uint32_t const width, uint32_t const height) -> void;

        /*template <typename Type = RenderPass, typename... Args>
        auto addPass(Args... args) -> void
        {
            RenderPassCreateInfo passCreateInfo;
            auto renderPass = core::make_ref<Type>(passCreateInfo, std::forward<Args>(args)...);
            renderPasses.emplace_back(std::move(renderPass));
        }*/

      private:
        core::ref_ptr<rhi::Device> device;

        core::ref_ptr<rhi::GraphicsContext> graphicsContext;
        core::ref_ptr<rhi::CopyContext> copyContext;

        core::ref_ptr<ConstantBufferPool> constantBufferPool;

        // std::vector<core::ref_ptr<RenderPass>> renderPasses;

        uint32_t outputWidth;
        uint32_t outputHeight;

        RenderQueue opaqueQueue;

        RenderQueue translucentQueue;
    };
} // namespace ionengine