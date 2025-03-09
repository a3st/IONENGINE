// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "camera.hpp"
#include "mesh.hpp"
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

    class Graphics
    {
      public:
        Graphics(core::ref_ptr<rhi::RHI> RHI, core::ref_ptr<internal::UploadManager> uploadManager);

        template <typename Type, typename... Args>
        auto addRenderPass(Args&&... args) -> RenderPass*
        {
            auto renderPass =
                std::make_unique<Type>(renderTargetsAllocator.get(), cameraTexture, std::forward<Args>(args)...);
            renderPathHash = renderPathHash == 0 ? renderPass->getHash() : renderPathHash ^ renderPass->getHash();
            RenderPass* outPass = renderPass.get();
            renderPasses.emplace_back(std::move(renderPass));
            return outPass;
        }

        auto beginFrame(core::ref_ptr<rhi::Texture> swapchainTexture) -> void;

        auto endFrame() -> void;

        auto drawMesh(core::ref_ptr<Mesh> drawableMesh, core::ref_ptr<Camera> targetCamera) -> void;

        auto createCamera(CameraViewType const viewType) -> core::ref_ptr<Camera>;

        auto loadShaderFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<Shader>;

        auto loadMeshFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<Mesh>;

      private:
        core::ref_ptr<rhi::RHI> RHI;
        core::ref_ptr<internal::UploadManager> uploadManager;

        core::ref_ptr<TextureAllocator> renderTargetsAllocator;
        core::ref_ptr<rhi::Texture> swapchainTexture;
        core::ref_ptr<BufferAllocator> buffersAllocator;
        core::ref_ptr<rhi::Texture> cameraTexture;

        using ResourceStateInfo = std::pair<core::ref_ptr<rhi::Texture>, rhi::ResourceState>;

        struct PassResourceData
        {
            std::list<ResourceStateInfo> readWriteResources;
            std::list<ResourceStateInfo> clearResources;
            core::ref_ptr<rhi::Buffer> passDataBuffer;
        };

        std::vector<core::ref_ptr<rhi::Sampler>> sharedSamplers;
        core::ref_ptr<rhi::Buffer> samplerDataBuffer;

        uint64_t renderPathHash;
        std::vector<std::unique_ptr<RenderPass>> renderPasses;
        std::unordered_map<uint64_t, std::vector<PassResourceData>> passResourcesCache;

        RenderQueue opaqueQueue;
        RenderQueue translucentQueue;

        core::ref_ptr<Shader> blitShader;

        auto findTextureInPassResources(std::span<PassResourceData const> const passResources, uint32_t const offset,
                                        core::ref_ptr<rhi::Texture> source,
                                        std::list<ResourceStateInfo>::const_iterator& it, rhi::ResourceState& state)
            -> int32_t;

        auto initializeSharedSamplers() -> void;
    };
} // namespace ionengine