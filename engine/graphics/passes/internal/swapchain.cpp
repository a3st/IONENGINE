// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "swapchain.hpp"
#include "precompiled.h"

namespace ionengine::passes
{
    SwapchainPass::SwapchainPass(TextureAllocator* textureAllocator, core::ref_ptr<rhi::Texture> cameraTexture,
                                 core::ref_ptr<Shader> shader, core::ref_ptr<rhi::Texture> swapchainTexture)
        : RenderPass("Swapchain Pass"), shader(shader)
    {
        rhi::RenderPassColorInfo const renderPassColorInfo{.texture = swapchainTexture,
                                                           .loadOp = rhi::RenderPassLoadOp::Clear,
                                                           .storeOp = rhi::RenderPassStoreOp::Store,
                                                           .clearColor = core::Color(0.0f, 0.0f, 0.0f, 1.0f)};
        this->colors.emplace_back(std::move(renderPassColorInfo));

        RenderPassInputInfo const renderPassInputInfo{.bindingName = "inputTexture1", .texture = cameraTexture};
        this->inputs.emplace_back(std::move(renderPassInputInfo));

        this->initializeRenderPass();
    }

    auto SwapchainPass::execute(RenderContext const& context, RenderableData const& renderableData) -> void
    {
        context.graphics->setGraphicsPipelineOptions(shader->getShader(), shader->getRasterizerStageInfo(),
                                                     shader->getBlendColorInfo(), std::nullopt);

        core::weak_ptr<rhi::Buffer> passDataBuffer;
        {
            auto const& passData = shader->getBindings().at("PASS_DATA");

            std::vector<uint8_t> passDataRawBuffer(passData.size);

            for (auto const& input : this->getInputs())
            {
                uint32_t const bindingOffset = shader->getBindings().at("PASS_DATA").elements.at(input.bindingName);
                uint32_t const descriptor = input.texture->getDescriptorOffset(rhi::TextureUsage::ShaderResource);

                std::memcpy(passDataRawBuffer.data() + bindingOffset, &descriptor, sizeof(uint32_t));
            }

            passDataBuffer = context.constBufferAllocator->allocate(passData.size);

            internal::UploadBufferInfo const uploadBufferInfo{
                .buffer = passDataBuffer.get(), .offset = 0, .dataBytes = passDataRawBuffer};
            context.uploadManager->uploadBuffer(uploadBufferInfo, []() -> void {});
        }

        uint32_t const passDataIndex =
            shader->getBindings().at("SHADER_DATA").elements.at("gPassData") / sizeof(uint32_t);

        context.graphics->bindDescriptor(passDataIndex,
                                         passDataBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));

        uint32_t const samplerDataIndex =
            shader->getBindings().at("SHADER_DATA").elements.at("gSamplerData") / sizeof(uint32_t);

        context.graphics->bindDescriptor(
            samplerDataIndex, context.samplerDataBuffer->getDescriptorOffset(rhi::BufferUsage::ConstantBuffer));

        context.graphics->draw(3, 1);
    }
} // namespace ionengine::passes