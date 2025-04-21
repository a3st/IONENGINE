// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include "txe/txe.hpp"

namespace ionengine
{
    class UploadManager;

    class Image : public core::ref_counted_object
    {
      public:
        Image(rhi::RHI& RHI, UploadManager& uploadManager, asset::TextureFile const& textureFile);

        Image(rhi::RHI& RHI, UploadManager& uploadManager, uint32_t const width, uint32_t const height,
              rhi::TextureFormat const format, std::span<uint8_t const> const dataBytes);

        auto getTexture() const -> core::ref_ptr<rhi::Texture>;

      private:
        core::ref_ptr<rhi::Texture> texture;
    };

    class RTImage : public core::ref_counted_object
    {
      public:
        RTImage(rhi::RHI& RHI, uint32_t const frameCount, uint32_t const width, uint32_t const height,
                rhi::TextureFormat const format);

        auto getTexture(uint32_t const frameIndex) const -> core::ref_ptr<rhi::Texture>;

      private:
        std::vector<core::ref_ptr<rhi::Texture>> textures;
    };
} // namespace ionengine