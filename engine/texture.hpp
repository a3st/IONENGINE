// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class LinkedDevice;

    enum class TextureFlags
    {
        None,
        RenderTarget,
        DepthStencil
    };

    class Texture : public Asset
    {
      public:
        Texture(LinkedDevice& device);

        auto create(uint32_t const width, uint32_t const height, TextureFlags const flags) -> bool;

        auto get_texture() const -> core::ref_ptr<rhi::Texture>;

        auto dump() -> std::vector<uint8_t>;

      private:
        LinkedDevice* device;
        core::ref_ptr<rhi::Texture> texture;
    };
} // namespace ionengine