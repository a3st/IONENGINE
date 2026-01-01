// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "attachment.hpp"
#include "precompiled.h"

namespace ionengine
{
    Attachment::Attachment(InternalAttachmentCreateInfo const& createInfo)
        : _width(createInfo.width), _height(createInfo.height), _format(createInfo.format),
          _dimension(createInfo.dimension), _flags(createInfo.flags), _isExternal(false)
    {
    }

    Attachment::Attachment(ExternalAttachmentCreateInfo const& createInfo)
        : _initialState(createInfo.initialState), _isExternal(true)
    {
    }

    auto Attachment::getWidth() const -> uint32_t
    {
        return _width;
    }

    auto Attachment::getHeight() const -> uint32_t
    {
        return _height;
    }

    auto Attachment::getFormat() const -> rhi::Format
    {
        return _format;
    }

    auto Attachment::getDimension() const -> rhi::TextureDimension
    {
        return _dimension;
    }

    auto Attachment::getFlags() const -> rhi::TextureUsageFlags
    {
        return _flags;
    }

    auto Attachment::isExternal() const -> bool
    {
        return _isExternal;
    }
} // namespace ionengine