// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "attachment.hpp"
#include "core/ref_ptr.hpp"
#include "subpass.hpp"
#include "texture_pool.hpp"

namespace ionengine
{
    class GraphicsContext
    {
      public:
        GraphicsContext(core::ref_ptr<Subpass> const& subpass,
                        std::unordered_map<std::string, rhi::Texture*>& boundAttachments);

        auto getTextureByName(std::string_view const attachmentName) const -> rhi::Texture*;

        auto getTextureFromColors(uint32_t const colorIndex) const -> rhi::Texture*;

      private:
        core::ref_ptr<Subpass> _subpass;
        std::unordered_map<std::string, rhi::Texture*>& _boundAttachments;
    };

    using ExecutePassHandler = std::function<void()>;

    class GraphicsPipeline : public core::ref_counted_object
    {
      public:
        GraphicsPipeline(std::vector<core::ref_ptr<Subpass>> const& subpasses,
                         std::unordered_map<std::string, core::ref_ptr<Attachment>> const& attachments,
                         std::unordered_map<std::string, std::vector<rhi::ResourceState>> const& attachmentTransitions);

        auto bindAttachment(std::string_view const attachmentName, rhi::Texture* texture) -> void;

        auto execute(core::ref_ptr<rhi::RHI> rhi, TexturePool& texturePool) -> rhi::Future<void>;

      private:
        std::vector<core::ref_ptr<Subpass>> _subpasses;
        std::unordered_map<std::string, core::ref_ptr<Attachment>> _attachments;
        std::unordered_map<std::string, rhi::Texture*> _boundAttachments;
        std::unordered_map<std::string, std::vector<rhi::ResourceState>> _attachmentTransitions;
    };

    class GraphicsPipelineBuilder
    {
        struct SubpassAttachmentInfo
        {
            uint32_t subpassIndex;
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