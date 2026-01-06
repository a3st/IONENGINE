// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics_pipeline.hpp"
#include "precompiled.h"

namespace ionengine
{
    GraphicsContext::GraphicsContext(core::ref_ptr<Subpass> subpass,
                                     std::unordered_map<std::string, rhi::Texture*>& boundAttachments)
        : _subpass(subpass), _boundAttachments(boundAttachments)
    {
    }

    auto GraphicsContext::getTextureByName(std::string_view const attachmentName) const -> rhi::Texture*
    {
        auto foundTexture = _boundAttachments.find(std::string(attachmentName));
        if (foundTexture != _boundAttachments.end())
        {
            return foundTexture->second;
        }
        else
        {
            return nullptr;
        }
    }

    auto GraphicsContext::getTextureFromColors(uint32_t const colorIndex) const -> rhi::Texture*
    {
        return nullptr;
    }

    GraphicsPipeline::GraphicsPipeline(
        std::vector<core::ref_ptr<Subpass>> const& subpasses,
        std::unordered_map<std::string, core::ref_ptr<Attachment>> const& attachments,
        std::unordered_map<std::string, std::vector<rhi::ResourceState>> const& attachmentTransitions)
        : _subpasses(subpasses), _attachments(attachments), _attachmentTransitions(attachmentTransitions),
          _renderWidth(800), _renderHeight(600)
    {
    }

    auto GraphicsPipeline::bindAttachment(std::string_view const attachmentName, rhi::Texture* texture) -> void
    {
        auto foundAttachment = _attachments.find(std::string(attachmentName));
        if (foundAttachment != _attachments.end())
        {
            if (foundAttachment->second->isExternal())
            {
                _boundAttachments[foundAttachment->first] = texture;
                _externalAttachments.emplace(foundAttachment->first);
            }
        }
    }

    auto GraphicsPipeline::execute(rhi::RHI& rhi, TexturePool& texturePool) -> rhi::Future<void>
    {
        for (uint32_t const i : std::views::iota(0u, _subpasses.size()))
        {
            _colorTextures.clear();
            rhi::Texture* depthStencilTexture = nullptr;

            auto const& subpass = _subpasses[i];

            for (auto const& input : subpass->getInputs())
            {
                auto& attachment = _attachments[input.name];

                auto boundResult = _boundAttachments.find(input.name);
                if (boundResult == _boundAttachments.end())
                {
                    assert(attachment->isExternal() && "Input attachment is not bound");

                    // Input attachments shouldn't be created here, they must be bound externally or from previous
                    // subpass
                    throw std::runtime_error("Input attachment is not bound: " + input.name);
                }

                tryAttachmentSubpassBarrier(rhi, *attachment, input.name, i, boundResult->second);

                // Try GC texture to pool if next subpasses doesn't use it
                if (i + 1 < _subpasses.size() && !attachment->isExternal())
                {
                    rhi::ResourceState const beforeState = _attachmentTransitions[input.name][i];
                    rhi::ResourceState const afterState = _attachmentTransitions[input.name][i + 1];

                    if (beforeState != afterState)
                    {
                        texturePool.deallocate(_attachmentAllocations[input.name], afterState);
                        boundResult->second = nullptr; // Set as null to avoid using deallocated texture
                    }
                }
            }

            for (auto const& color : subpass->getColors())
            {
                rhi::Texture* colorTexture = nullptr;

                auto& attachment = _attachments[color.name];

                auto boundResult = _boundAttachments.find(color.name);
                if (boundResult != _boundAttachments.end())
                {
                    colorTexture = boundResult->second;
                }

                if (!colorTexture)
                {
                    uint32_t width = _renderWidth;
                    uint32_t height = _renderHeight;

                    if (std::holds_alternative<AttachmentRelativeSize>(attachment->getSize()))
                    {
                        auto& attachmentRelativeSize = std::get<AttachmentRelativeSize>(attachment->getSize());
                        width *= attachmentRelativeSize.widthFactor;
                        height *= attachmentRelativeSize.heightFactor;
                    }
                    else if (std::holds_alternative<AttachmentAbsoluteSize>(attachment->getSize()))
                    {
                        auto& attachmentAbsoluteSize = std::get<AttachmentAbsoluteSize>(attachment->getSize());
                        width = attachmentAbsoluteSize.width;
                        height = attachmentAbsoluteSize.height;
                    }

                    rhi::TextureCreateInfo const textureCreateInfo{.width = width,
                                                                   .height = height,
                                                                   .depth = 1,
                                                                   .mipLevels = 1,
                                                                   .format = attachment->getFormat(),
                                                                   .dimension = attachment->getDimension(),
                                                                   .flags = attachment->getFlags()};
                    auto allocationResult = texturePool.allocate(textureCreateInfo);
                    if (allocationResult.has_value())
                    {
                        TexturePool::Allocation textureAllocation = allocationResult.value();
                        colorTexture = textureAllocation.getTexture();
                        _boundAttachments[color.name] = colorTexture;
                        _attachmentAllocations[color.name] = std::move(textureAllocation);
                    }
                    else
                    {
                        throw std::runtime_error("Failed to allocate texture for color attachment: " + color.name);
                    }
                }

                tryAttachmentSubpassBarrier(rhi, *attachment, color.name, i, colorTexture);

                _colorTextures.emplace_back(colorTexture);
            }

            subpass->beginPass(rhi.getGraphicsContext(), _colorTextures, depthStencilTexture);

            // Execture Handler

            subpass->endPass(rhi.getGraphicsContext());
        }

        uint32_t const lastSubpassIndex = static_cast<uint32_t>(_subpasses.size() - 1);

        // Deallocate TexturePool textures for non-external attachments
        for (auto const& [attachmentName, textureAllocation] : _attachmentAllocations)
        {
            texturePool.deallocate(_attachmentAllocations[attachmentName],
                                   _attachmentTransitions[attachmentName][lastSubpassIndex]);
        }

        texturePool.compact();

        // Transition external attachments to their initial states
        for (auto const& attachmentName : _externalAttachments)
        {
            auto& attachment = _attachments[attachmentName];

            auto boundResult = _boundAttachments.find(attachmentName);
            if (boundResult != _boundAttachments.end())
            {
                rhi::ResourceState const beforeState = _attachmentTransitions[attachmentName][lastSubpassIndex];
                rhi::ResourceState const afterState = attachment->getInitialState();

                if (beforeState != afterState)
                {
                    rhi.getGraphicsContext()->barrier(boundResult->second, beforeState, afterState);
                }
            }
        }

        auto executeResult = rhi.getGraphicsContext()->execute();

        _boundAttachments.clear();
        _attachmentAllocations.clear();
        _externalAttachments.clear();

        return executeResult;
    }

    auto GraphicsPipelineBuilder::addSubpass(SubpassCreateInfo const& createInfo, ExecutePassHandler&& executeHandler)
        -> GraphicsPipelineBuilder&
    {
        if (_subpassNames.count(createInfo.name))
        {
            return *this;
        }

        auto subpass = core::make_ref<Subpass>(createInfo);

        uint32_t const subpassIndex = static_cast<uint32_t>(_subpasses.size());

        for (uint32_t const i : std::views::iota(0u, createInfo.colors.size()))
        {
            SubpassAttachmentInfo attachmentInfo{.subpassIndex = subpassIndex, .colorIndex = static_cast<int32_t>(i)};

            _attachments.try_emplace(createInfo.colors[i].name);
            _subpassAttachments[createInfo.colors[i].name].emplace_back(std::move(attachmentInfo));
        }

        if (createInfo.depthStencil.has_value())
        {
            auto depthStencilValue = createInfo.depthStencil.value();

            SubpassAttachmentInfo attachmentInfo{.subpassIndex = subpassIndex, .colorIndex = -1};

            _attachments.try_emplace(depthStencilValue.name);
            _subpassAttachments[depthStencilValue.name].emplace_back(std::move(attachmentInfo));
            _depthStencilNames.emplace(depthStencilValue.name);
        }

        for (auto const& input : createInfo.inputs)
        {
            SubpassAttachmentInfo attachmentInfo{.subpassIndex = subpassIndex, .colorIndex = -2};

            _attachments.try_emplace(input.name);
            _subpassAttachments[input.name].emplace_back(std::move(attachmentInfo));
        }

        _subpasses.emplace_back(subpass);
        _subpassNames.emplace(createInfo.name);
        return *this;
    }

    auto GraphicsPipelineBuilder::build() -> core::ref_ptr<GraphicsPipeline>
    {
        std::unordered_map<std::string, std::vector<rhi::ResourceState>> attachmentTransitions;

        for (auto const& [attachmentName, attachmentInfos] : _subpassAttachments)
        {
            attachmentTransitions.try_emplace(attachmentName);
            uint32_t previousSubpassIndex = 0;

            for (auto const& resourceInfo : attachmentInfos)
            {
                rhi::ResourceState newState;
                switch (resourceInfo.colorIndex)
                {
                    // DepthStencil variant
                    case -1: {
                        newState = rhi::ResourceState::DepthStencilWrite;
                        break;
                    }
                    // Input variant
                    case -2: {
                        newState = rhi::ResourceState::ShaderRead;
                        break;
                    }
                    default: {
                        // Color variant
                        if (resourceInfo.colorIndex >= 0)
                        {
                            newState = rhi::ResourceState::RenderTarget;
                        }
                        else
                        {
                            throw std::invalid_argument("Unknown kind of resource");
                        }
                    }
                }

                if (resourceInfo.subpassIndex > 0)
                {
                    rhi::ResourceState const previousState = attachmentTransitions[attachmentName].back();

                    for (uint32_t const i : std::views::iota(previousSubpassIndex, resourceInfo.subpassIndex))
                    {
                        attachmentTransitions[attachmentName].emplace_back(previousState);
                    }
                }

                attachmentTransitions[attachmentName].emplace_back(newState);
                previousSubpassIndex = resourceInfo.subpassIndex;
            }
        }
        return core::make_ref<GraphicsPipeline>(_subpasses, _attachments, attachmentTransitions);
    }

    auto GraphicsPipelineBuilder::addAttachment(AttachmentCreateInfo const& createInfo) -> GraphicsPipelineBuilder&
    {
        if (std::holds_alternative<InternalAttachmentCreateInfo>(createInfo))
        {
            auto& internalAttachmentCreateInfo = std::get<InternalAttachmentCreateInfo>(createInfo);

            if (_attachments.count(internalAttachmentCreateInfo.name))
            {
                return *this;
            }

            _attachments.emplace(internalAttachmentCreateInfo.name,
                                 core::make_ref<Attachment>(internalAttachmentCreateInfo));
        }
        else if (std::holds_alternative<ExternalAttachmentCreateInfo>(createInfo))
        {
            auto& externalAttachmentCreateInfo = std::get<ExternalAttachmentCreateInfo>(createInfo);

            if (_attachments.count(externalAttachmentCreateInfo.name))
            {
                return *this;
            }

            _attachments.emplace(externalAttachmentCreateInfo.name,
                                 core::make_ref<Attachment>(externalAttachmentCreateInfo));
        }
        return *this;
    }

    auto GraphicsPipeline::tryAttachmentSubpassBarrier(rhi::RHI& rhi, Attachment& attachment,
                                                       std::string_view const attachmentName,
                                                       uint32_t const subpassIndex, rhi::Texture* texture) -> uint32_t
    {
        uint32_t barrierCount = 0;

        rhi::ResourceState const beforeState =
            subpassIndex == 0 ? attachment.getInitialState()
                              : _attachmentTransitions[std::string(attachmentName)][subpassIndex - 1];
        rhi::ResourceState const afterState = _attachmentTransitions[std::string(attachmentName)][subpassIndex];

        if (beforeState != afterState)
        {
            rhi.getGraphicsContext()->barrier(texture, beforeState, afterState);
            barrierCount++;
        }
        return barrierCount;
    }

    auto GraphicsPipeline::setRenderSize(uint32_t const width, uint32_t const height) -> void
    {
    }
} // namespace ionengine