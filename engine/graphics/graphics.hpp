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
    class Graphics
    {
      public:
        Graphics(core::ref_ptr<rhi::RHI> RHI);

        auto beginFrame(core::ref_ptr<rhi::Texture> swapchainTexture) -> void;

        auto endFrame() -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;
        std::unique_ptr<internal::UploadManager> uploadManager;

        core::ref_ptr<TextureAllocator> renderTargetsAllocator;
        core::ref_ptr<rhi::Texture> swapchainTexture;
        core::ref_ptr<BufferAllocator> constBuffersAllocator;
        core::ref_ptr<Camera> targetCamera;

        core::ref_ptr<Shader> base3DShader;

        using ResourceStateInfo =
            std::pair<core::ref_ptr<rhi::Texture>, std::pair<rhi::ResourceState, rhi::ResourceState>>;

        struct PassResourceData
        {
            std::list<ResourceStateInfo> readWriteResources;
            std::list<ResourceStateInfo> clearResources;
            core::ref_ptr<rhi::Buffer> passDataBuffer;
        };

        std::vector<core::ref_ptr<rhi::Sampler>> sharedSamplers;
        core::ref_ptr<rhi::Buffer> samplerDataBuffer;

        uint64_t renderPathHash;
        std::function<void()> renderPathUpdated;
        std::vector<std::unique_ptr<RenderPass>> renderPasses;
        std::unordered_map<uint64_t, std::vector<PassResourceData>> passResourcesCache;

        RenderQueue opaqueQueue;
        RenderQueue translucentQueue;
        std::set<core::ref_ptr<Camera>> targetCameras;

        core::ref_ptr<Shader> blitShader;

        auto findTextureInPassResources(std::span<PassResourceData const> const passResources, uint32_t const offset,
                                        core::ref_ptr<rhi::Texture> source,
                                        std::list<ResourceStateInfo>::const_iterator& it, rhi::ResourceState& state)
            -> int32_t;

        auto initializeSharedSamplers() -> void;

        inline static Graphics* instance;

      public:
        template <typename Type, typename... Args>
        static auto addRenderPass(Args&&... args) -> RenderPass*
        {
            auto renderPass = std::make_unique<Type>(instance->renderTargetsAllocator.get(),
                                                     instance->targetCamera->getTexture(), std::forward<Args>(args)...);
            instance->renderPathHash = instance->renderPathHash == 0 ? renderPass->getHash()
                                                                     : instance->renderPathHash ^ renderPass->getHash();
            RenderPass* outPass = renderPass.get();
            instance->renderPasses.emplace_back(std::move(renderPass));
            return outPass;
        }

        static auto drawMesh(core::ref_ptr<Mesh> drawableMesh, core::Mat4f const& modelMatrix,
                             core::ref_ptr<Camera> targetCamera) -> void;

        static auto createPerspectiveCamera(float const fovy, float const zNear, float const zFar)
            -> core::ref_ptr<PerspectiveCamera>;

        static auto loadShaderFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<Shader>;

        static auto loadMeshFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<Mesh>;

        static auto setRenderPath(std::function<void()>&& func) -> void;
    };
} // namespace ionengine