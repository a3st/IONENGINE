// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics_pipeline.hpp"
#include "precompiled.h"

namespace ionengine
{
    GraphicsContext::GraphicsContext(core::ref_ptr<Subpass> const& subpass,
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
        : _subpasses(subpasses), _attachments(attachments), _attachmentTransitions(attachmentTransitions)
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
            }
        }
    }

    auto GraphicsPipeline::execute(core::ref_ptr<rhi::RHI> rhi, TexturePool& texturePool) -> rhi::Future<void>
    {
        for (uint32_t const i : std::views::iota(0u, _subpasses.size()))
        {
            auto const& subpass = _subpasses[i];

            std::vector<rhi::Texture*> colorTextures;
            rhi::Texture* depthStencilTexture = nullptr;

            for (auto const& input : subpass->getInputs())
            {
                rhi::Texture* inputTexture = nullptr;

                auto boundResult = _boundAttachments.find(input.name);
                if (boundResult != _boundAttachments.end())
                {
                    inputTexture = boundResult->second;
                }
                else
                {
                    auto& attachment = _attachments[input.name];
                    assert(attachment->isExternal() && "Input attachment is not bound");

                    // Input attachments shouldn't be created here, they must be bound externally or from previous
                    // subpass
                    throw std::runtime_error("Input attachment is not bound: " + input.name);
                }

                rhi::ResourceState beforeState;
                rhi::ResourceState afterState = _attachmentTransitions[input.name][i];
                if (i == 0)
                {
                    beforeState = rhi::ResourceState::Common;
                }
                else
                {
                    beforeState = _attachmentTransitions[input.name][i - 1];
                }

                if (beforeState != afterState)
                {
                    rhi->getGraphicsContext()->barrier(inputTexture, beforeState, afterState);
                }

                // Try GC texture to pool if next subpasses doesn't use it
                if(i + 1 < _subpasses.size())
                {
                    
                }
            }

            for (auto const& color : subpass->getColors())
            {
                rhi::Texture* colorTexture = nullptr;

                auto boundResult = _boundAttachments.find(color.name);
                if (boundResult != _boundAttachments.end())
                {
                    colorTexture = boundResult->second;
                }
                else
                {
                    auto& attachment = _attachments[color.name];
                    assert(attachment->isExternal() && "Color attachment is not bound");

                    rhi::TextureCreateInfo const textureCreateInfo{.width = attachment->getWidth(),
                                                                   .height = attachment->getHeight(),
                                                                   .depth = 1,
                                                                   .mipLevels = 1,
                                                                   .format = attachment->getFormat(),
                                                                   .dimension = attachment->getDimension(),
                                                                   .flags = attachment->getFlags()};

                    auto allocationResult = texturePool.allocate(textureCreateInfo);
                    if (allocationResult.has_value())
                    {
                        colorTexture = allocationResult->getTexture();
                        _boundAttachments[color.name] = colorTexture;
                    }
                    else
                    {
                        throw std::runtime_error("Failed to allocate texture for color attachment: " + color.name);
                    }
                }

                rhi::ResourceState beforeState;
                rhi::ResourceState afterState = _attachmentTransitions[color.name][i];
                if (i == 0)
                {
                    beforeState = rhi::ResourceState::Common;
                }
                else
                {
                    beforeState = _attachmentTransitions[color.name][i - 1];
                }

                if (beforeState != afterState)
                {
                    rhi->getGraphicsContext()->barrier(colorTexture, beforeState, afterState);
                }
            }
        }

        // Clear bound attachments at end execute
        _boundAttachments.clear();

        return rhi::Future<void>();
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

                for (uint32_t const i : std::views::iota(previousSubpassIndex, resourceInfo.subpassIndex + 1))
                {
                    attachmentTransitions[attachmentName].emplace_back(newState);
                }

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
} // namespace ionengine