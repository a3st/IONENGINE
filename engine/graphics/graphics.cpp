// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "graphics.hpp"
#include "passes/internal/swapchain.hpp"
#include "precompiled.h"

namespace ionengine
{
    Graphics::Graphics(core::ref_ptr<rhi::RHI> RHI)
        : RHI(RHI), uploadManager(std::make_unique<internal::UploadManager>(RHI)),
          renderTargetsAllocator(core::make_ref<TextureAllocator>(RHI)),
          constBuffersAllocator(core::make_ref<BufferAllocator>(RHI)), renderPathHash(0)
    {
        std::string shaderExt;
        if (RHI->getName().compare("D3D12") == 0)
        {
            shaderExt = "_pc";
        }
        else
        {
            shaderExt = "_vk";
        }

        blitShader = this->loadShaderFromFile("engine/shaders/blit_t1" + shaderExt + ".bin");
        base3DShader = this->loadShaderFromFile("../../assets/shaders/base3d_test_color" + shaderExt + ".bin");

        this->initializeSharedSamplers();
    }

    auto Graphics::loadShaderFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<Shader>
    {
        auto shaderResult =
            core::deserialize<core::serialize_iarchive, asset::ShaderFile, std::basic_ifstream<uint8_t>>(
                std::basic_ifstream<uint8_t>(filePath, std::ios::binary));
        if (shaderResult.has_value())
        {
            return core::make_ref<Shader>(*RHI, shaderResult.value());
        }
        else
        {
            return nullptr;
        }
    }

    auto Graphics::loadMeshFromFile(std::filesystem::path const& filePath) -> core::ref_ptr<Mesh>
    {
        auto meshResult = core::deserialize<core::serialize_iarchive, asset::ModelFile, std::basic_ifstream<uint8_t>>(
            std::basic_ifstream<uint8_t>(filePath, std::ios::binary));
        if (meshResult.has_value())
        {
            return core::make_ref<Mesh>(*RHI, uploadManager.get(), meshResult.value());
        }
        else
        {
            return nullptr;
        }
    }

    auto Graphics::beginFrame(core::ref_ptr<rhi::Texture> swapchainTexture) -> void
    {
        renderTargetsAllocator->reset();
        constBuffersAllocator->reset();
        renderPasses.clear();
        renderPathHash = 0;
        this->swapchainTexture = swapchainTexture;

        opaqueQueue.clear();
        translucentQueue.clear();
        targetCameras.clear();
    }

    auto Graphics::endFrame() -> void
    {
        for (auto const& targetCamera : targetCameras)
        {
            this->targetCamera = targetCamera;

            renderPathUpdated();
        }

        if (renderPasses.empty())
        {
            return;
        }

        this->addRenderPass<passes::SwapchainPass>(blitShader, swapchainTexture);

        auto pathResult = passResourcesCache.find(renderPathHash);
        if (pathResult == passResourcesCache.end())
        {
            std::vector<PassResourceData> passResources;
            passResources.resize(renderPasses.size());

            std::cout << "Compiling render path" << std::endl;

            std::unordered_set<core::ref_ptr<rhi::Texture>> trackingResources;

            bool isNeedUploadBuffer = false;

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
                    rhi::BufferCreateInfo const bufferCreateInfo{
                        .size = 256,
                        .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest};
                    passResources[i].passDataBuffer = RHI->createBuffer(bufferCreateInfo);
                    std::vector<uint8_t> passDataRawBuffer(256);

                    for (auto const& input : renderPasses[i]->getInputs())
                    {
                        uint32_t const bindingOffset =
                            renderPasses[i]->getShader()->getBindings().at("PASS_DATA").elements.at(input.bindingName);
                        uint32_t const descriptor =
                            input.texture->getDescriptorOffset(rhi::TextureUsage::ShaderResource);

                        std::memcpy(passDataRawBuffer.data() + bindingOffset, &descriptor, sizeof(uint32_t));

                        passResources[i].readWriteResources.emplace_back(
                            input.texture, std::make_pair(rhi::ResourceState::Common, rhi::ResourceState::ShaderRead));
                        trackingResources.emplace(input.texture);
                    }

                    RHI->getCopyContext()->updateBuffer(passResources[i].passDataBuffer, 0, passDataRawBuffer);

                    isNeedUploadBuffer = true;
                }
            }

            if (isNeedUploadBuffer)
            {
                RHI->getCopyContext()->execute();
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

        auto graphicsContext = RHI->getGraphicsContext();

        graphicsContext->setViewport(0, 0, swapchainTexture->getWidth(), swapchainTexture->getHeight());
        graphicsContext->setScissor(0, 0, swapchainTexture->getWidth(), swapchainTexture->getHeight());

        for (uint32_t const i : std::views::iota(0u, renderPasses.size()))
        {
            auto& curPassCache = passResourcesCache[renderPathHash][i];

            for (auto const& readWriteResource : curPassCache.readWriteResources)
            {
                graphicsContext->barrier(readWriteResource.first, readWriteResource.second.first,
                                         readWriteResource.second.second);
            }

            graphicsContext->beginRenderPass(renderPasses[i]->getColors(), std::nullopt);

            if (renderPasses[i]->getShader())
            {
                graphicsContext->setGraphicsPipelineOptions(renderPasses[i]->getShader()->getShader(),
                                                            renderPasses[i]->getShader()->getRasterizerStageInfo(),
                                                            rhi::BlendColorInfo::Opaque(), std::nullopt);

                if (curPassCache.passDataBuffer)
                {
                    uint32_t const passDataIndex =
                        renderPasses[i]->getShader()->getBindings().at("SHADER_DATA").elements.at("gPassData") /
                        sizeof(uint32_t);

                    graphicsContext->bindDescriptor(passDataIndex, curPassCache.passDataBuffer->getDescriptorOffset(
                                                                       rhi::BufferUsage::ConstantBuffer));
                }

                uint32_t const samplerDataIndex =
                    renderPasses[i]->getShader()->getBindings().at("SHADER_DATA").elements.at("gSamplerData") /
                    sizeof(uint32_t);

                graphicsContext->bindDescriptor(
                    samplerDataIndex, samplerDataBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));
            }

            RenderContext const renderContext{.graphics = graphicsContext,
                                              .uploadManager = uploadManager.get(),
                                              .constBufferAllocator = constBuffersAllocator.get()};
            RenderableData const renderableData{.opaqueQueue = &opaqueQueue,
                                                .viewMat = targetCamera->getViewMatrix(),
                                                .projMat = targetCamera->getProjMatrix()};

            renderPasses[i]->execute(renderContext, renderableData);

            graphicsContext->endRenderPass();

            uploadManager->onExecuteTask(true, true);

            for (auto const& clearResource : curPassCache.clearResources)
            {
                graphicsContext->barrier(clearResource.first, clearResource.second.first, clearResource.second.second);
            }
        }

        uploadManager->onExecuteTask(false, false);

        graphicsContext->execute().wait();
    }

    auto Graphics::drawMesh(core::ref_ptr<Mesh> drawableMesh, core::Mat4f const& modelMatrix,
                            core::ref_ptr<Camera> targetCamera) -> void
    {
        if (drawableMesh->isUploaded())
        {
            for (auto const& surface : drawableMesh->getSurfaces())
            {
                DrawableData drawableData{
                    .surface = surface, .shader = base3DShader, .modelMat = modelMatrix, .layerIndex = 0};
                opaqueQueue.push(std::move(drawableData));
            }
        }

        targetCameras.emplace(targetCamera);
    }

    auto Graphics::createPerspectiveCamera(float const fovy, float const zNear, float const zFar)
        -> core::ref_ptr<PerspectiveCamera>
    {
        return core::make_ref<PerspectiveCamera>(*RHI, fovy, zNear, zFar);
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

    auto Graphics::initializeSharedSamplers() -> void
    {
        {
            rhi::SamplerCreateInfo const samplerCreateInfo{.filter = rhi::Filter::Anisotropic,
                                                           .addressU = rhi::AddressMode::Wrap,
                                                           .addressV = rhi::AddressMode::Wrap,
                                                           .addressW = rhi::AddressMode::Wrap,
                                                           .compareOp = rhi::CompareOp::LessEqual,
                                                           .maxAnisotropy = 4};
            sharedSamplers.emplace_back(std::move(RHI->createSampler(samplerCreateInfo)));
        }

        rhi::BufferCreateInfo const bufferCreateInfo{
            .size = 256, .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest};
        samplerDataBuffer = RHI->createBuffer(bufferCreateInfo);

        std::vector<uint8_t> samplerDataRawBuffer(256);

        for (auto const& sampler : sharedSamplers)
        {
            uint32_t const descriptor = sampler->getDescriptorOffset();
            std::memcpy(samplerDataRawBuffer.data(), &descriptor, sizeof(uint32_t));
        }

        RHI->getCopyContext()->updateBuffer(samplerDataBuffer, 0, samplerDataRawBuffer);
        RHI->getCopyContext()->execute().wait();
    }
} // namespace ionengine