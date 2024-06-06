// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/asset.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class LinkedDevice;

    enum class TextureUsage
    {
        None,
        RenderTarget,
        DepthStencil
    };

    class TextureAsset : public Asset
    {
      public:
        TextureAsset(LinkedDevice& device);

        auto create(uint32_t const width, uint32_t const height, TextureUsage const usage) -> bool;

        auto getTexture() const -> core::ref_ptr<rhi::Texture>;

        auto dump() -> std::vector<uint8_t>;

      private:
        LinkedDevice* device;
        core::ref_ptr<rhi::Texture> texture;
    };
} // namespace ionengine