// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset.hpp"
#include "importers/ktx2_image.hpp"
#include "renderer/renderer.hpp"

namespace ionengine
{
    enum class TextureFormat
    {
        KTX2
    };

    class Texture : public Asset
    {
      public:
        Texture(renderer::Renderer& renderer);

        auto load(std::span<uint8_t const> const data_bytes, TextureFormat const format, bool const immediate = false)
            -> bool;

        auto get_texture() -> core::ref_ptr<renderer::Texture>
        {
            return texture;
        }

      private:
        renderer::Renderer* renderer;
        core::ref_ptr<renderer::Texture> texture;
    };
} // namespace ionengine