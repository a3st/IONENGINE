// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class Subpass;

    class GraphicsContext
    {
      public:
        GraphicsContext(core::ref_ptr<Subpass> subpass,
                        std::unordered_map<std::string, rhi::Texture*>& boundAttachments);

        auto getTextureFromColors(uint32_t const colorIndex) const -> rhi::Texture*;

        auto getTextureFromDepthStencil() const -> rhi::Texture*;

      private:
        core::ref_ptr<Subpass> _subpass;
        std::unordered_map<std::string, rhi::Texture*>& _boundAttachments;

        auto getTextureByName(std::string_view const attachmentName) const -> rhi::Texture*;
    };
} // namespace ionengine