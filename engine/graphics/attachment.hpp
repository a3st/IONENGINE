// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    struct ExternalAttachmentCreateInfo
    {
        std::string name;
        rhi::ResourceState initialState;
    };

    struct InternalAttachmentCreateInfo
    {
        std::string name;
        uint32_t width;
        uint32_t height;
        rhi::Format format;
        rhi::TextureDimension dimension;
        rhi::TextureUsageFlags flags;

        static auto RenderTarget2D(std::string_view const name, uint32_t const width, uint32_t const height,
                                   rhi::Format const format) -> InternalAttachmentCreateInfo
        {
            return {.name = std::string(name),
                    .width = width,
                    .height = height,
                    .format = format,
                    .dimension = rhi::TextureDimension::_2D,
                    .flags = rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource |
                             rhi::TextureUsage::CopySource};
        }

        static auto DepthStencil(std::string_view const name, uint32_t const width, uint32_t const height,
                                 rhi::Format const format) -> InternalAttachmentCreateInfo
        {
            return {.name = std::string(name),
                    .width = width,
                    .height = height,
                    .format = format,
                    .dimension = rhi::TextureDimension::_2D,
                    .flags = rhi::TextureUsage::DepthStencil | rhi::TextureUsage::ShaderResource |
                             rhi::TextureUsage::CopySource};
        }
    };

    using AttachmentCreateInfo = std::variant<InternalAttachmentCreateInfo, ExternalAttachmentCreateInfo>;

    class Attachment : public core::ref_counted_object
    {
      public:
        explicit Attachment(InternalAttachmentCreateInfo const& createInfo);

        explicit Attachment(ExternalAttachmentCreateInfo const& createInfo);

        auto getWidth() const -> uint32_t;

        auto getHeight() const -> uint32_t;

        auto getFormat() const -> rhi::Format;

        auto getDimension() const -> rhi::TextureDimension;

        auto getFlags() const -> rhi::TextureUsageFlags;

        auto isExternal() const -> bool;

      private:
        uint32_t _width;
        uint32_t _height;
        rhi::Format _format;
        rhi::TextureDimension _dimension;
        rhi::TextureUsageFlags _flags;
        rhi::ResourceState _initialState;
        bool _isExternal;
    };
} // namespace ionengine