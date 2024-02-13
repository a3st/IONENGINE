// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "texture.hpp"
#include "precompiled.h"

namespace ionengine
{
    Texture::Texture(renderer::Renderer& renderer) : renderer(&renderer)
    {
    }

    auto Texture::load(std::span<uint8_t const> const data_bytes, TextureFormat const format, bool const immediate)
        -> bool
    {
        switch (format)
        {
            case TextureFormat::KTX2: {
                ktx2::Ktx2Image image(data_bytes);

                auto image_data = image.get_image_data();

                std::vector<std::span<uint8_t const>> mip_data;
                for (uint32_t const i : std::views::iota(0u, image_data.level_count))
                {
                    mip_data.emplace_back(image.get_mip_data(i));
                }

                texture = renderer->create_texture(image_data.pixel_width, image_data.pixel_height, 1,
                                                   image_data.level_count, renderer::rhi::TextureFormat::BC3,
                                                   renderer::rhi::TextureDimension::_2D, mip_data);
                break;
            }
        }
        return true;
    }
} // namespace ionengine