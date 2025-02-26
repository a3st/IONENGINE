// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/matrix.hpp"
#include "render_pass.hpp"
#include "render_queue.hpp"
#include "rhi/rhi.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace ionengine
{
    /*enum class RenderLayers
    {
        Opaque = 0,
        Translucent = 1
    };

    struct RenderData
    {
        std::vector<RenderQueue> objectQueues;
        core::ref_ptr<rhi::Texture> targetTexture;
        math::Mat4f view;
        math::Mat4f projection;
    };*/

    class Graphics : public core::ref_counted_object
    {
      public:
        Graphics(core::ref_ptr<rhi::RHI> RHI);

        template <typename Type, typename... Args>
        auto addRenderPass(Args&&... args) -> RenderPass*
        {
            auto renderPass =
                std::make_unique<Type>(renderTargetsAllocator.get(), nullptr, std::forward<Args>(args)...);
            renderPathHash = renderPathHash == 0 ? renderPass->getHash() : renderPathHash ^ renderPass->getHash();

            RenderPass* outPass = renderPass.get();
            RenderPathData renderPathData{.renderPass = std::move(renderPass)};
            renderPaths.emplace_back(std::move(renderPathData));
            return outPass;
        }

        auto beginFrame(core::ref_ptr<rhi::Texture> swapchainTexture) -> void;

        auto endFrame() -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;

        core::ref_ptr<TextureAllocator> renderTargetsAllocator;
        core::ref_ptr<rhi::Texture> swapchainTexture;

        using ResourceStateInfo = std::pair<core::ref_ptr<rhi::Texture>, rhi::ResourceState>;

        struct RenderPathData
        {
            std::list<ResourceStateInfo> readWriteResources;
            std::list<ResourceStateInfo> clearResources;
            std::unique_ptr<RenderPass> renderPass;
        };

        uint64_t renderPathHash;
        std::vector<RenderPathData> renderPaths;

        std::unordered_map<uint64_t, std::vector<RenderPathData>> renderPathsCache;

        core::ref_ptr<Shader> blitShader;

        auto findTextureInRenderPasses(uint32_t const offset, core::ref_ptr<rhi::Texture> source,
                                       std::list<ResourceStateInfo>::iterator& it, rhi::ResourceState& state)
            -> int32_t;
    };
} // namespace ionengine