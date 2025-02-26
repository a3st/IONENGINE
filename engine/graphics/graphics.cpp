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

        auto shaderResult =
            core::deserialize<core::serialize_iarchive, asset::ShaderFile, std::basic_ifstream<uint8_t>>(
                std::basic_ifstream<uint8_t>("engine/shaders/blit_t1" + shaderExt + ".bin", std::ios::binary));
        blitShader = core::make_ref<Shader>(*RHI, shaderResult.value());
    }

    auto Graphics::beginFrame(core::ref_ptr<rhi::Texture> swapchainTexture) -> void
    {
        renderTargetsAllocator->reset();
        renderPaths = std::vector<RenderPathData>{};
        renderPathHash = 0;
        this->swapchainTexture = swapchainTexture;
    }

    auto Graphics::endFrame() -> void
    {
        this->addRenderPass<passes::SwapchainPass>(blitShader, swapchainTexture);

        auto pathResult = renderPathsCache.find(renderPathHash);
        if (pathResult == renderPathsCache.end())
        {
            renderPathsCache[renderPathHash] = std::move(renderPaths);

            std::cout << "Compiling render path" << std::endl;

            std::set<core::ref_ptr<rhi::Texture>> trackingResources;

            for (auto& pathData : renderPathsCache[renderPathHash])
            {
                for (auto& color : pathData.renderPass->getColors())
                {
                    if (color.loadOp == rhi::RenderPassLoadOp::Clear || color.loadOp == rhi::RenderPassLoadOp::Load)
                    {
                        pathData.readWriteResources.emplace_back(color.texture, rhi::ResourceState::RenderTarget);
                        trackingResources.emplace(color.texture);
                    }
                }
            }

            for (auto const& trackingResource : trackingResources)
            {
                uint32_t offset = 0, prevOffset = 0;
                uint32_t distance = 0;
                std::list<ResourceStateInfo>::iterator it;
                rhi::ResourceState state, prevState;

                while ((distance = this->findTextureInRenderPasses(offset, trackingResource, it, state)) != -1)
                {
                    if (prevOffset != offset && state == prevState)
                    {
                        renderPathsCache[renderPathHash][offset].readWriteResources.erase(it);
                    }

                    prevOffset = offset;
                    prevState = state;

                    std::cout << offset << std::endl;
                    offset += distance + 1;
                }

                renderPathsCache[renderPathHash][prevOffset].clearResources.emplace_back(trackingResource, prevState);
            }
        }

        auto graphicsContext = RHI->getGraphicsContext();

        graphicsContext->setViewport(0, 0, swapchainTexture->getWidth(), swapchainTexture->getHeight());
        graphicsContext->setScissor(0, 0, swapchainTexture->getWidth(), swapchainTexture->getHeight());

        for (auto& pathData : renderPathsCache[renderPathHash])
        {
            for (auto const& readWriteResource : pathData.readWriteResources)
            {
                graphicsContext->barrier(readWriteResource.first, rhi::ResourceState::Common, readWriteResource.second);
            }

            graphicsContext->beginRenderPass(pathData.renderPass->getColors(), std::nullopt);

            pathData.renderPass->execute(graphicsContext);

            graphicsContext->endRenderPass();

            for (auto const& clearResource : pathData.clearResources)
            {
                graphicsContext->barrier(clearResource.first, clearResource.second, rhi::ResourceState::Common);
            }
        }

        auto execResult = graphicsContext->execute();
        execResult.wait();
    }

    auto Graphics::findTextureInRenderPasses(uint32_t const offset, core::ref_ptr<rhi::Texture> source,
                                             std::list<ResourceStateInfo>::iterator& it, rhi::ResourceState& state)
        -> int32_t
    {
        auto findTextureRecursive = [this](core::ref_ptr<rhi::Texture> source, uint32_t offset, uint32_t& distance,
                                           std::list<ResourceStateInfo>::iterator& it,
                                           rhi::ResourceState& state) -> bool {
            auto findTextureRecursiveImpl = [this](core::ref_ptr<rhi::Texture> source, uint32_t offset,
                                                   uint32_t& distance, std::list<ResourceStateInfo>::iterator& it,
                                                   rhi::ResourceState& state, auto& implRef) mutable -> bool {
                if (offset >= renderPathsCache[renderPathHash].size())
                {
                    return false;
                }

                for (auto resIt = renderPathsCache[renderPathHash][offset].readWriteResources.begin();
                     resIt != renderPathsCache[renderPathHash][offset].readWriteResources.end(); ++resIt)
                {
                    if (resIt->first == source)
                    {
                        it = resIt;
                        state = resIt->second;
                        return true;
                    }
                }

                return implRef(source, ++offset, ++distance, it, state, implRef);
            };

            return findTextureRecursiveImpl(source, offset, distance, it, state, findTextureRecursiveImpl);
        };

        uint32_t distance = 0;
        return findTextureRecursive(source, offset, distance, it, state) ? distance : -1;
    }
} // namespace ionengine