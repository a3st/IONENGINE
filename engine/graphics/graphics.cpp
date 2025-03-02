// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "graphics.hpp"
#include "passes/internal/swapchain.hpp"
#include "precompiled.h"

namespace ionengine
{
    Graphics::Graphics(core::ref_ptr<rhi::RHI> RHI)
        : RHI(RHI), renderTargetsAllocator(core::make_ref<TextureAllocator>(RHI)), renderPathHash(0)
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

        // For Tests
        rhi::TextureCreateInfo const textureCreateInfo{
            .width = 800,
            .height = 600,
            .depth = 1,
            .mipLevels = 1,
            .dimension = rhi::TextureDimension::_2D,
            .flags = (rhi::TextureUsageFlags)(rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource)};
        cameraTexture = RHI->createTexture(textureCreateInfo);
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

    auto Graphics::beginFrame(core::ref_ptr<rhi::Texture> swapchainTexture) -> void
    {
        renderTargetsAllocator->reset();
        buffersAllocator->reset();
        renderPasses.clear();
        renderPathHash = 0;
        this->swapchainTexture = swapchainTexture;
    }

    auto Graphics::endFrame() -> void
    {
        this->addRenderPass<passes::SwapchainPass>(blitShader, swapchainTexture);

        auto pathResult = passResourcesCache.find(renderPathHash);
        if (pathResult == passResourcesCache.end())
        {
            std::vector<PassResourceData> passResources;
            passResources.resize(renderPasses.size());

            std::cout << "Compiling render path" << std::endl;

            std::set<core::ref_ptr<rhi::Texture>> trackingResources;

            bool isNeedUploadBuffer = false;

            for (uint32_t const i : std::views::iota(0u, renderPasses.size()))
            {
                for (auto& color : renderPasses[i]->getColors())
                {
                    if (color.loadOp == rhi::RenderPassLoadOp::Clear || color.loadOp == rhi::RenderPassLoadOp::Load)
                    {
                        passResources[i].readWriteResources.emplace_back(color.texture,
                                                                         rhi::ResourceState::RenderTarget);
                        trackingResources.emplace(color.texture);
                    }
                }

                if (!renderPasses[i]->getInputs().empty())
                {
                    passResources[i].passDataIndex =
                        renderPasses[i]->getShader()->getBindOffsets().at("SHADER_DATA.passData") / sizeof(uint32_t);

                    rhi::BufferCreateInfo const bufferCreateInfo{
                        .size = 256,
                        .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest};
                    passResources[i].passDataBuffer = RHI->createBuffer(bufferCreateInfo);
                    std::vector<uint8_t> passDataRawBuffer(256);

                    for (auto const& input : renderPasses[i]->getInputs())
                    {
                        uint32_t const bindingOffset =
                            renderPasses[i]->getShader()->getBindOffsets().at("PASS_DATA." + input.bindingName);
                        std::memcpy(passDataRawBuffer.data() + bindingOffset, &bindingOffset, sizeof(uint32_t));
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
                std::list<ResourceStateInfo>::const_iterator it;
                rhi::ResourceState state, prevState;

                while ((distance =
                            this->findTextureInPassResources(passResources, offset, trackingResource, it, state)) != -1)
                {
                    if (prevOffset != offset && state == prevState)
                    {
                        passResources[offset].readWriteResources.erase(it);
                    }

                    prevOffset = offset;
                    prevState = state;

                    offset += distance + 1;
                }

                passResources[prevOffset].clearResources.emplace_back(trackingResource, prevState);
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
                graphicsContext->barrier(readWriteResource.first, rhi::ResourceState::Common, readWriteResource.second);
            }

            graphicsContext->beginRenderPass(renderPasses[i]->getColors(), std::nullopt);

            graphicsContext->setGraphicsPipelineOptions(renderPasses[i]->getShader()->getShader(),
                                                        renderPasses[i]->getShader()->getRasterizerStageInfo(),
                                                        rhi::BlendColorInfo::Opaque(), std::nullopt);

            if (curPassCache.passDataBuffer)
            {
                graphicsContext->bindDescriptor(
                    curPassCache.passDataIndex,
                    curPassCache.passDataBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));
            }

            renderPasses[i]->execute(graphicsContext);

            graphicsContext->endRenderPass();

            for (auto const& clearResource : curPassCache.clearResources)
            {
                graphicsContext->barrier(clearResource.first, clearResource.second, rhi::ResourceState::Common);
            }
        }

        auto execResult = graphicsContext->execute();
        execResult.wait();
    }

    auto Graphics::drawMesh(core::ref_ptr<Mesh> drawable, core::ref_ptr<Camera> targetCamera) -> void
    {
    }

    auto Graphics::createCamera() -> core::ref_ptr<Camera>
    {
        return nullptr;
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
                        state = resIt->second;
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
} // namespace ionengine