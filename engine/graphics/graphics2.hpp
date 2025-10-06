// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer_allocator.hpp"
#include "camera.hpp"
#include "image.hpp"
#include "material.hpp"
#include "mesh.hpp"
#include "render_pass.hpp"
#include "render_queue.hpp"
#include "rhi/rhi.hpp"
#include "texture_allocator.hpp"
#include "upload_manager.hpp"
#include "world.hpp"

namespace ionengine
{
    enum class DrawType
    {
        Triangles
    };

    struct DrawParameters
    {
        DrawType drawType;
        core::ref_ptr<Surface> surface;
        core::ref_ptr<Material> material;
        RenderGroup renderGroup;
        core::Mat4f modelMatrix;
        std::optional<core::Mat4f> viewMatrix;
        std::optional<core::Mat4f> projMatrix;
        std::optional<core::Recti> scissorRect;
    };

    class Graphics
    {
      public:
        Graphics(core::ref_ptr<rhi::RHI> RHI, uint32_t const numBuffering);

        auto beginFrame() -> void;

        auto endFrame() -> void;

        auto onResize(uint32_t const width, uint32_t const height) -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;
        std::unique_ptr<UploadManager> uploadManager;

        struct FrameResourceData
        {
            core::ref_ptr<TextureAllocator> renderTargetAllocator;
            core::ref_ptr<BufferAllocator> constBufferAllocator;
            rhi::Future<void> graphicsResult;
            std::set<core::ref_ptr<Material>> usedMaterials;
            std::set<core::ref_ptr<Surface>> usedSurfaces;
            std::set<core::ref_ptr<Camera>> usedCameras;
        };

        std::vector<FrameResourceData> frameResources;
        uint32_t frameIndex;

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

        std::unordered_map<uint64_t, core::ref_ptr<Surface>> surfacesCache;

        core::ref_ptr<rhi::Texture> curSwapchainTexture;
        core::ref_ptr<rhi::Texture> curTargetTexture;
        std::unordered_map<RenderGroup, RenderQueue>* curRenderGroups;

        uint32_t outputWidth;
        uint32_t outputHeight;
        bool isOutputResized;

        core::ref_ptr<Shader> blitShader;

        auto findTextureInPassResources(std::span<PassResourceData const> const passResources, uint32_t const offset,
                                        core::ref_ptr<rhi::Texture> source,
                                        std::list<ResourceStateInfo>::const_iterator& it, rhi::ResourceState& state)
            -> int32_t;

        auto cacheRenderPasses() -> void;

        auto executeRenderPasses() -> void;

        auto initializeSharedSamplers() -> void;

        inline static Graphics* instance;

      public:
        template <typename Type, typename... Args>
        static auto addRenderPass(Args&&... args) -> RenderPass*
        {
            assert(instance->curTargetTexture);

            auto renderPass =
                std::make_unique<Type>(*instance->frameResources[instance->frameIndex].renderTargetAllocator,
                                       instance->curTargetTexture, std::forward<Args>(args)...);
            renderPass->renderGroups = instance->curRenderGroups;
            instance->renderPathHash = instance->renderPathHash == 0 ? renderPass->getHash()
                                                                     : instance->renderPathHash ^ renderPass->getHash();
            RenderPass* outPass = renderPass.get();
            instance->renderPasses.emplace_back(std::move(renderPass));
            return outPass;
        }

        template <typename Func>
        static auto setRenderPath(Func&& function) -> void
        {
            instance->renderPathUpdated = function;
        }

        static auto drawGeometry(DrawParameters const& drawParams,
                                 std::span<core::ref_ptr<Camera> const> const drawCameras) -> void;

        static auto drawWorld(core::ref_ptr<World> const& world) -> void;

        static auto createPerspectiveCamera(float const fovy, float const zNear, float const zFar)
            -> core::ref_ptr<PerspectiveCamera>;

        static auto createShader(std::filesystem::path const& filePath) -> core::ref_ptr<Shader>;

        static auto createMesh(std::filesystem::path const& filePath) -> core::ref_ptr<Mesh>;

        static auto createImage(std::filesystem::path const& filePath) -> core::ref_ptr<Image>;

        static auto createImage(uint32_t const width, uint32_t const height, rhi::TextureFormat const format,
                                std::span<uint8_t const> const dataBytes) -> core::ref_ptr<Image>;

        static auto createMaterial(core::ref_ptr<Shader> const& shader) -> core::ref_ptr<Material>;

        static auto createSurface(std::span<uint8_t const> const vertexDataBytes,
                                  std::span<uint8_t const> const indexDataBytes) -> core::ref_ptr<Surface>;

        static auto createDirectionalLight(core::Vec3f const& direction) -> core::ref_ptr<DirectionalLight>;

        static auto createWorld() -> core::ref_ptr<World>;
    };
} // namespace ionengine