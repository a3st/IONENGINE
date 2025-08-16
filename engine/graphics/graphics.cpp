// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "graphics.hpp"
#include "passes/internal/swapchain.hpp"
#include "passes/internal/ui.hpp"
#include "precompiled.h"

namespace ionengine
{
    Graphics::Graphics(core::ref_ptr<rhi::RHI> RHI, uint32_t const numBuffering)
        : RHI(RHI), uploadManager(std::make_unique<UploadManager>(RHI, numBuffering)), renderPathHash(0), frameIndex(0),
          outputWidth(800), outputHeight(600), isOutputResized(false)
    {
        instance = this;

        std::string shaderExt;
        if (RHI->getName().compare("D3D12") == 0)
        {
            shaderExt = "_pc";
        }
        else
        {
            shaderExt = "_vk";
        }

        blitShader = this->createShader("../../assets/shaders/blit_t1" + shaderExt + ".bin");

        this->initializeSharedSamplers();

        // Initialize per Frame Resources
        for (uint32_t const i : std::views::iota(0u, numBuffering))
        {
            FrameResourceData frameResourceData{
                .renderTargetAllocator = core::make_ref<TextureAllocator>(RHI),
                .constBufferAllocator = core::make_ref<BufferAllocator>(RHI, rhi::BufferUsage::ConstantBuffer)};
            frameResources.emplace_back(std::move(frameResourceData));
        }
    }

    auto Graphics::createShader(std::filesystem::path const& filePath) -> core::ref_ptr<Shader>
    {
        auto shaderResult =
            core::deserialize<core::serialize_iarchive, asset::ShaderFile, std::basic_ifstream<uint8_t>>(
                std::basic_ifstream<uint8_t>(filePath, std::ios::binary));
        if (shaderResult.has_value())
        {
            return core::make_ref<Shader>(*instance->RHI, shaderResult.value());
        }
        else
        {
            return nullptr;
        }
    }

    auto Graphics::createMesh(std::filesystem::path const& filePath) -> core::ref_ptr<Mesh>
    {
        auto modelResult = core::deserialize<core::serialize_iarchive, asset::ModelFile>(
            std::basic_ifstream<uint8_t>(filePath, std::ios::binary));

        if (modelResult.has_value())
        {
            return core::make_ref<Mesh>(*instance->RHI, *instance->uploadManager, instance->surfacesCache,
                                        modelResult.value());
        }
        else
        {
            return nullptr;
        }
    }

    auto Graphics::createImage(std::filesystem::path const& filePath) -> core::ref_ptr<Image>
    {
        auto imageResult =
            core::deserialize<core::serialize_iarchive, asset::TextureFile, std::basic_ifstream<uint8_t>>(
                std::basic_ifstream<uint8_t>(filePath, std::ios::binary));
        if (imageResult.has_value())
        {
            return core::make_ref<Image>(*instance->RHI, *instance->uploadManager, imageResult.value());
        }
        else
        {
            return nullptr;
        }
    }

    auto Graphics::beginFrame() -> void
    {
        auto& curFrameResourceData = frameResources[frameIndex];
        curFrameResourceData.graphicsResult.wait();

        curSwapchainTexture = RHI->getSwapchain()->getBackBuffer().get();

        curFrameResourceData.renderTargetAllocator->reset();
        curFrameResourceData.constBufferAllocator->reset();
        curFrameResourceData.usedMaterials.clear();
        curFrameResourceData.usedSurfaces.clear();

        for (auto& camera : curFrameResourceData.usedCameras)
        {
            for (auto& [renderGroup, renderQueue] : camera->renderGroups)
            {
                renderQueue.clear();
            }
        }
        curFrameResourceData.usedCameras.clear();

        renderPathHash = 0;
        renderPasses.clear();
    }

    auto Graphics::endFrame() -> void
    {
        auto& curFrameResourceData = frameResources[frameIndex];

        core::ref_ptr<rhi::Texture> curMainTargetTexture;

        for (auto const& camera : curFrameResourceData.usedCameras)
        {
            if (camera->getTargetImage())
            {
                curTargetTexture = camera->getTargetImage()->getTexture(frameIndex);
            }
            else
            {
                rhi::TextureCreateInfo const textureCreateInfo{
                    .width = outputWidth,
                    .height = outputHeight,
                    .depth = 1,
                    .mipLevels = 1,
                    .format = rhi::TextureFormat::RGBA8_UNORM,
                    .dimension = rhi::TextureDimension::_2D,
                    .flags =
                        (rhi::TextureUsageFlags)(rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource)};
                curTargetTexture = frameResources[frameIndex].renderTargetAllocator->allocate(textureCreateInfo).get();

                curMainTargetTexture = curTargetTexture;
            }

            curRenderGroups = &camera->renderGroups;

            this->renderPathUpdated();
        }

        curTargetTexture = curMainTargetTexture;
        this->addRenderPass<passes::SwapchainPass>(blitShader, curSwapchainTexture);

        // Free variable
        curTargetTexture = nullptr;

        this->cacheRenderPasses();
        this->executeRenderPasses();

        uploadManager->onExecute();
        RHI->getGraphicsContext()->execute();

        frameResources[frameIndex].graphicsResult = RHI->getSwapchain()->presentBackBuffer();

        // TODO! Fix Double Buffering Bug
        // frameIndex = (frameIndex + 1) % static_cast<uint32_t>(frameResources.size());

        // Free variable
        curSwapchainTexture = nullptr;
    }

    auto Graphics::onResize(uint32_t const width, uint32_t const height) -> void
    {
        renderPasses.clear();
        passResourcesCache.clear();

        RHI->getSwapchain()->resizeBackBuffers(width, height);

        for (auto const& frameResource : frameResources)
        {
            frameResource.renderTargetAllocator->flush();
        }

        outputWidth = width;
        outputHeight = height;

        isOutputResized = true;
    }

    auto Graphics::drawGeometry(DrawParameters const& drawParams,
                                std::span<core::ref_ptr<Camera> const> const drawCameras) -> void
    {
        auto& curFrameResourceData = instance->frameResources[instance->frameIndex];
        curFrameResourceData.usedMaterials.emplace(drawParams.material);
        curFrameResourceData.usedSurfaces.emplace(drawParams.surface);

        drawParams.material->updateEffectDataBuffer(*instance->uploadManager, instance->frameIndex);

        for (auto const& camera : drawCameras)
        {
            curFrameResourceData.usedCameras.emplace(camera);

            core::weak_ptr<rhi::Buffer> transformDataBuffer;
            {
                auto const& transformData = drawParams.material->getShader()->getBindings().at("TRANSFORM_DATA");

                uint32_t const modelViewProjOffset =
                    drawParams.material->getShader()->getBindings().at("TRANSFORM_DATA").elements.at("modelViewProj");

                std::vector<uint8_t> transformDataRawBuffer(transformData.size);

                core::Mat4f const modelViewProjMatrix = drawParams.modelMatrix *
                                                        drawParams.viewMatrix.value_or(camera->getViewMatrix()) *
                                                        drawParams.projMatrix.value_or(camera->getProjMatrix());
                std::memcpy(transformDataRawBuffer.data() + modelViewProjOffset, modelViewProjMatrix.data(),
                            sizeof(core::Mat4f));

                transformDataBuffer = curFrameResourceData.constBufferAllocator->allocate(transformData.size);

                UploadBufferInfo const uploadBufferInfo{
                    .buffer = transformDataBuffer.get(), .offset = 0, .dataBytes = transformDataRawBuffer};
                instance->uploadManager->uploadBuffer(uploadBufferInfo);
            }

            DrawableData drawableData{.surface = drawParams.surface,
                                      .shader = drawParams.material->getShader(),
                                      .effectDataBuffer =
                                          drawParams.material->getEffectDataBuffer(instance->frameIndex),
                                      .transformDataBuffer = transformDataBuffer.get(),
                                      .scissorRect = drawParams.scissorRect};
            camera->renderGroups[drawParams.renderGroup].push(std::move(drawableData));
        }
    }

    auto Graphics::drawWorld(core::ref_ptr<World> const& world) -> void
    {
        auto& curFrameResourceData = instance->frameResources[instance->frameIndex];

        // world->sortMeshes();

        for (auto const& [_, camera] : world->getCameras())
        {
            if (instance->isOutputResized)
            {
                if (auto perspectiveCamera = dynamic_cast<PerspectiveCamera*>(camera.get()))
                {
                    perspectiveCamera->setAspect((float)instance->outputWidth / instance->outputHeight);
                }
            }

            curFrameResourceData.usedCameras.emplace(camera);

            for (auto const& [_, mesh] : world->getMeshes())
            {
                // Frustum
                if (true)
                {
                    for (uint32_t const i : std::views::iota(0u, mesh->getSurfaces().size()))
                    {
                        core::ref_ptr<Material> currentMaterial = mesh->getMaterials()[i];
                        core::ref_ptr<Surface> currentSurface = mesh->getSurfaces()[i];

                        curFrameResourceData.usedMaterials.emplace(currentMaterial);
                        curFrameResourceData.usedSurfaces.emplace(currentSurface);

                        currentMaterial->updateEffectDataBuffer(*instance->uploadManager, instance->frameIndex);

                        core::weak_ptr<rhi::Buffer> transformDataBuffer;
                        {
                            auto const& transformData =
                                currentMaterial->getShader()->getBindings().at("TRANSFORM_DATA");

                            uint32_t const modelViewProjOffset = currentMaterial->getShader()
                                                                     ->getBindings()
                                                                     .at("TRANSFORM_DATA")
                                                                     .elements.at("modelViewProj");

                            std::vector<uint8_t> transformDataRawBuffer(transformData.size);

                            core::Mat4f const modelMatrix =
                                core::Mat4f::translate(mesh->position) * core::Mat4f::scale(mesh->scale);
                            core::Mat4f const modelViewProjMatrix =
                                modelMatrix * camera->getViewMatrix() * camera->getProjMatrix();
                            std::memcpy(transformDataRawBuffer.data() + modelViewProjOffset, modelViewProjMatrix.data(),
                                        sizeof(core::Mat4f));

                            transformDataBuffer =
                                instance->frameResources[instance->frameIndex].constBufferAllocator->allocate(
                                    transformData.size);

                            UploadBufferInfo const uploadBufferInfo{
                                .buffer = transformDataBuffer.get(), .offset = 0, .dataBytes = transformDataRawBuffer};
                            instance->uploadManager->uploadBuffer(uploadBufferInfo);
                        }

                        DrawableData drawableData{.surface = currentSurface,
                                                  .shader = currentMaterial->getShader(),
                                                  .effectDataBuffer =
                                                      currentMaterial->getEffectDataBuffer(instance->frameIndex),
                                                  .transformDataBuffer = transformDataBuffer.get()};
                        camera->renderGroups[RenderGroup::Opaque].push(std::move(drawableData));
                    }
                }
            }
        }

        if (instance->isOutputResized)
        {
            instance->isOutputResized = false;
        }
    }

    auto Graphics::createPerspectiveCamera(float const fovy, float const zNear, float const zFar)
        -> core::ref_ptr<PerspectiveCamera>
    {
        return core::make_ref<PerspectiveCamera>(fovy, (float)instance->outputWidth / instance->outputHeight, zNear,
                                                 zFar);
    }

    auto Graphics::createMaterial(core::ref_ptr<Shader> const& shader) -> core::ref_ptr<Material>
    {
        return core::make_ref<Material>(*instance->RHI, static_cast<uint32_t>(instance->frameResources.size()), shader);
    }

    auto Graphics::findTextureInPassResources(std::span<PassResourceData const> const passResources,
                                              uint32_t const offset, core::ref_ptr<rhi::Texture> source,
                                              std::list<ResourceStateInfo>::const_iterator& it,
                                              rhi::ResourceState& state) -> int32_t
    {
        auto findTextureRecursive = [this](std::span<PassResourceData const> const passResources,
                                           core::ref_ptr<rhi::Texture> source, uint32_t offset, uint32_t& distance,
                                           std::list<ResourceStateInfo>::const_iterator& it,
                                           rhi::ResourceState& state) -> bool {
            auto findTextureRecursiveImpl = [this](std::span<PassResourceData const> const passResources,
                                                   core::ref_ptr<rhi::Texture> source, uint32_t offset,
                                                   uint32_t& distance, std::list<ResourceStateInfo>::const_iterator& it,
                                                   rhi::ResourceState& state, auto& implRef) mutable -> bool {
                if (offset >= passResources.size())
                {
                    return false;
                }

                for (auto resIt = passResources[offset].readWriteResources.begin();
                     resIt != passResources[offset].readWriteResources.end(); ++resIt)
                {
                    if (resIt->first == source)
                    {
                        it = resIt;
                        state = resIt->second.second;
                        return true;
                    }
                }

                return implRef(passResources, source, ++offset, ++distance, it, state, implRef);
            };

            return findTextureRecursiveImpl(passResources, source, offset, distance, it, state,
                                            findTextureRecursiveImpl);
        };

        uint32_t distance = 0;
        return findTextureRecursive(passResources, source, offset, distance, it, state) ? distance : -1;
    }

    auto Graphics::cacheRenderPasses() -> void
    {
        auto pathResult = passResourcesCache.find(renderPathHash);
        if (pathResult == passResourcesCache.end())
        {
            std::vector<PassResourceData> passResources;
            passResources.resize(renderPasses.size());

            std::cout << "Compiling render path" << std::endl;

            std::unordered_set<core::ref_ptr<rhi::Texture>> trackingResources;

            for (uint32_t const i : std::views::iota(0u, renderPasses.size()))
            {
                for (auto& color : renderPasses[i]->getColors())
                {
                    if (color.loadOp == rhi::RenderPassLoadOp::Clear || color.loadOp == rhi::RenderPassLoadOp::Load)
                    {
                        passResources[i].readWriteResources.emplace_back(
                            color.texture,
                            std::make_pair(rhi::ResourceState::Common, rhi::ResourceState::RenderTarget));
                        trackingResources.emplace(color.texture);
                    }
                }

                if (!renderPasses[i]->getInputs().empty())
                {
                    for (auto const& input : renderPasses[i]->getInputs())
                    {
                        passResources[i].readWriteResources.emplace_back(
                            input.texture, std::make_pair(rhi::ResourceState::Common, rhi::ResourceState::ShaderRead));
                        trackingResources.emplace(input.texture);
                    }
                }
            }

            for (auto const& trackingResource : trackingResources)
            {
                uint32_t offset = 0, prevOffset = 0;
                uint32_t distance = 0;
                std::list<ResourceStateInfo>::iterator it;
                rhi::ResourceState state = rhi::ResourceState::Common, prevState = rhi::ResourceState::Common;

                while ((distance =
                            this->findTextureInPassResources(passResources, offset, trackingResource, it, state)) != -1)
                {
                    if (prevOffset != offset)
                    {
                        if (state == prevState)
                        {
                            passResources[offset].readWriteResources.erase(it);
                        }
                        else
                        {
                            it->second.first = prevState;
                        }
                    }

                    prevOffset = offset + distance;
                    prevState = state;

                    offset = prevOffset + (distance == 0 ? 1 : distance);
                }

                passResources[prevOffset].clearResources.emplace_back(
                    trackingResource, std::make_pair(prevState, rhi::ResourceState::Common));
            }

            passResourcesCache[renderPathHash] = std::move(passResources);
        }
    }

    auto Graphics::executeRenderPasses() -> void
    {
        for (uint32_t const i : std::views::iota(0u, renderPasses.size()))
        {
            auto& curPassCache = passResourcesCache[renderPathHash][i];

            for (auto const& readWriteResource : curPassCache.readWriteResources)
            {
                RHI->getGraphicsContext()->barrier(readWriteResource.first, readWriteResource.second.first,
                                                   readWriteResource.second.second);
            }

            RHI->getGraphicsContext()->beginRenderPass(renderPasses[i]->getColors(), std::nullopt);

            RenderContext const renderContext{.graphics = RHI->getGraphicsContext(),
                                              .uploadManager = uploadManager.get(),
                                              .constBufferAllocator =
                                                  frameResources[frameIndex].constBufferAllocator.get(),
                                              .samplerDataBuffer = samplerDataBuffer};
            renderPasses[i]->execute(renderContext);

            RHI->getGraphicsContext()->endRenderPass();

            for (auto const& clearResource : curPassCache.clearResources)
            {
                RHI->getGraphicsContext()->barrier(clearResource.first, clearResource.second.first,
                                                   clearResource.second.second);
            }
        }
    }

    auto Graphics::initializeSharedSamplers() -> void
    {
        rhi::SamplerCreateInfo const samplerCreateInfo{.filter = rhi::Filter::Anisotropic,
                                                       .addressU = rhi::AddressMode::Wrap,
                                                       .addressV = rhi::AddressMode::Wrap,
                                                       .addressW = rhi::AddressMode::Wrap,
                                                       .compareOp = rhi::CompareOp::LessEqual,
                                                       .maxAnisotropy = 4};
        sharedSamplers.emplace_back(RHI->createSampler(samplerCreateInfo));

        rhi::BufferCreateInfo const bufferCreateInfo{
            .size = 256, .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest};
        samplerDataBuffer = RHI->createBuffer(bufferCreateInfo);

        std::vector<uint8_t> samplerDataRawBuffer(256);

        for (auto const& sampler : sharedSamplers)
        {
            uint32_t const descriptor = sampler->getDescriptorOffset();
            std::memcpy(samplerDataRawBuffer.data(), &descriptor, sizeof(uint32_t));
        }

        UploadBufferInfo const uploadBufferInfo{
            .buffer = samplerDataBuffer, .offset = 0, .dataBytes = samplerDataRawBuffer};
        uploadManager->uploadBuffer(uploadBufferInfo);
    }

    auto Graphics::createSurface(std::span<uint8_t const> const vertexDataBytes,
                                 std::span<uint8_t const> const indexDataBytes) -> core::ref_ptr<Surface>
    {

        core::ref_ptr<rhi::Buffer> vertexBuffer;
        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = vertexDataBytes.size(),
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};
            vertexBuffer = instance->RHI->createBuffer(bufferCreateInfo);
        }

        core::ref_ptr<rhi::Buffer> indexBuffer;
        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = indexDataBytes.size(),
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Index | rhi::BufferUsage::CopyDest)};
            indexBuffer = instance->RHI->createBuffer(bufferCreateInfo);
        }

        auto surface = core::make_ref<Surface>(vertexBuffer, indexBuffer, indexDataBytes.size() / sizeof(uint32_t));

        {
            UploadBufferInfo const uploadBufferInfo{.buffer = vertexBuffer, .offset = 0, .dataBytes = vertexDataBytes};
            instance->uploadManager->uploadBuffer(uploadBufferInfo);
        }

        {
            UploadBufferInfo const uploadBufferInfo{.buffer = indexBuffer, .offset = 0, .dataBytes = indexDataBytes};
            instance->uploadManager->uploadBuffer(uploadBufferInfo);
        }

        return surface;
    }

    auto Graphics::createImage(uint32_t const width, uint32_t const height, rhi::TextureFormat const format,
                               std::span<uint8_t const> const dataBytes) -> core::ref_ptr<Image>
    {
        return core::make_ref<Image>(*instance->RHI, *instance->uploadManager, width, height, format, dataBytes);
    }

    auto Graphics::createDirectionalLight(core::Vec3f const& direction) -> core::ref_ptr<DirectionalLight>
    {
        return core::make_ref<DirectionalLight>();
    }

    auto Graphics::createWorld() -> core::ref_ptr<World>
    {
        return core::make_ref<World>();
    }
} // namespace ionengine