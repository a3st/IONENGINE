// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "attachment.hpp"
#include "precompiled.h"

namespace ionengine
{
    Attachment::Attachment(InternalAttachmentCreateInfo const& createInfo)
        : _size(createInfo.size), _format(createInfo.format), _dimension(createInfo.dimension),
          _flags(createInfo.flags), _initialState(rhi::ResourceState::Common), _isExternal(false)
    {
    }

    Attachment::Attachment(ExternalAttachmentCreateInfo const& createInfo)
        : _initialState(createInfo.initialState), _isExternal(true)
    {
    }

    auto Attachment::getSize() const -> AttachmentSize const&
    {
        return _size;
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

    auto Attachment::getInitialState() const -> rhi::ResourceState
    {
        return _initialState;
    }

    auto Attachment::isExternal() const -> bool
    {
        return _isExternal;
    }
} // namespace ionengine