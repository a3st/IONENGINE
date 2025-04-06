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

        auto getTexture() const -> core::ref_ptr<rhi::Texture>;

      private:
        core::ref_ptr<rhi::Texture> texture;
    };
} // namespace ionengine