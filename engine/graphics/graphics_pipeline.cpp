// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics_pipeline.hpp"
#include "precompiled.h"

namespace ionengine
{
    GraphicsPipeline::GraphicsPipeline(
        std::vector<core::ref_ptr<Subpass>> const& subpasses,
        std::unordered_map<std::string, core::ref_ptr<Attachment>> const& attachments,
        std::unordered_map<std::string, std::vector<std::optional<rhi::ResourceState>>> const& attachmentTransitions)
        : _subpasses(subpasses), _attachments(attachments), _attachmentTransitions(attachmentTransitions)
    {
    }

    auto GraphicsPipeline::bindAttachment(std::string_view const attachmentName,
                                          core::ref_ptr<rhi::Texture> boundTexture) -> void
    {
    }

    auto GraphicsPipelineBuilder::addSubpass(SubpassCreateInfo const& createInfo, ExecutePassHandler&& executeHandler)
        -> GraphicsPipelineBuilder&
    {
        if (_subpassNames.count(createInfo.name))
        {
            return *this;
        }

        auto subpass = core::make_ref<Subpass>(createInfo);

        for (uint32_t const i : std::views::iota(0u, createInfo.colors.size()))
        {
            SubpassAttachmentInfo attachmentInfo{.subpass = subpass, .colorIndex = static_cast<int32_t>(i)};

            _attachments.try_emplace(createInfo.colors[i].name);
            _subpassAttachments[createInfo.colors[i].name].emplace_back(std::move(attachmentInfo));
        }

        if (createInfo.depthStencil.has_value())
        {
            auto depthStencilValue = createInfo.depthStencil.value();

            SubpassAttachmentInfo attachmentInfo{.subpass = subpass, .colorIndex = -1};

            _attachments.try_emplace(depthStencilValue.name);
            _subpassAttachments[depthStencilValue.name].emplace_back(std::move(attachmentInfo));
            _depthStencilNames.emplace(depthStencilValue.name);
        }

        for (auto const& input : createInfo.inputs)
        {
            SubpassAttachmentInfo attachmentInfo{.subpass = subpass, .colorIndex = -2};

            _attachments.try_emplace(input.name);
            _subpassAttachments[input.name].emplace_back(std::move(attachmentInfo));
        }

        _subpasses.emplace_back(subpass);
        _subpassNames.emplace(createInfo.name);
        return *this;
    }

    auto GraphicsPipelineBuilder::build() -> core::ref_ptr<GraphicsPipeline>
    {
        std::unordered_map<std::string, std::vector<std::optional<rhi::ResourceState>>> attachmentTransitions;

        for (auto const& [attachmentName, attachmentInfos] : _subpassAttachments)
        {
            attachmentTransitions.try_emplace(attachmentName);
            std::optional<rhi::ResourceState> previousState;

            for (auto const& resourceInfo : attachmentInfos)
            {
                std::optional<rhi::ResourceState> newState;
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

                bool isDuplicate = (previousState == newState);
                attachmentTransitions[attachmentName].emplace_back(isDuplicate ? std::nullopt : newState);
                if (!isDuplicate)
                {
                    previousState = newState;
                }
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