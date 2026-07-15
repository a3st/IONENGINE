// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "graphics_context.hpp"
#include "precompiled.h"
#include "subpass.hpp"

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
        if (colorIndex < _subpass->getColors().size())
        {
            return this->getTextureByName(_subpass->getColors()[colorIndex].name);
        }
        else
        {
            return nullptr;
        }
    }

    auto GraphicsContext::getTextureFromDepthStencil() const -> rhi::Texture*
    {
        if (_subpass->getDepthStencil().has_value())
        {
            auto depthStencilValue = _subpass->getDepthStencil().value();
            return this->getTextureByName(depthStencilValue.name);
        }
        else
        {
            return nullptr;
        }
    }
} // namespace ionengine