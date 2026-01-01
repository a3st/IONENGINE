// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "attachment.hpp"
#include "core/ref_ptr.hpp"
#include "subpass.hpp"

namespace ionengine
{
    using ExecutePassHandler = std::function<void()>;

    class GraphicsPipeline : public core::ref_counted_object
    {
      public:
        GraphicsPipeline(
            std::vector<core::ref_ptr<Subpass>> const& subpasses,
            std::unordered_map<std::string, core::ref_ptr<Attachment>> const& attachments,
            std::unordered_map<std::string, std::vector<std::optional<rhi::ResourceState>>> const& attachmentTransitions);

        auto bindAttachment(std::string_view const attachmentName, core::ref_ptr<rhi::Texture> boundTexture) -> void;

        auto execute(core::ref_ptr<rhi::RHI> rhi) -> rhi::Future<void>;

      private:
        std::vector<core::ref_ptr<Subpass>> _subpasses;
        std::unordered_map<std::string, core::ref_ptr<Attachment>> _attachments;
        std::unordered_map<std::string, core::ref_ptr<rhi::Texture>> _boundAttachments;
		std::unordered_map<std::string, std::vector<std::optional<rhi::ResourceState>>> _attachmentTransitions;
    };

    class GraphicsPipelineBuilder
    {
        struct SubpassAttachmentInfo
        {
            core::ref_ptr<Subpass> subpass;
            int32_t colorIndex; // > 0 - Colors, -1 - DepthStencil, -2 - Inputs
        };

      public:
        GraphicsPipelineBuilder() = default;

        auto addAttachment(AttachmentCreateInfo const& createInfo) -> GraphicsPipelineBuilder&;

        auto addSubpass(SubpassCreateInfo const& createInfo, ExecutePassHandler&& executeHandler)
            -> GraphicsPipelineBuilder&;

        auto build() -> core::ref_ptr<GraphicsPipeline>;

      private:
        std::vector<core::ref_ptr<Subpass>> _subpasses;
        std::unordered_map<std::string, core::ref_ptr<Attachment>> _attachments;
        std::unordered_set<std::string> _subpassNames;
        std::unordered_map<std::string, std::vector<SubpassAttachmentInfo>> _subpassAttachments;
        std::unordered_set<std::string> _depthStencilNames;
    };
} // namespace ionengine