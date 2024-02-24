// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/exception.hpp"
#include "rhi/texture.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>



namespace ionengine::renderer::rhi
{
    

    inline auto get_texture_data(rhi::TextureFormat const format, uint32_t const mip_width, uint32_t const mip_height,
                                 size_t& row_bytes, uint32_t& row_count) -> void
    {
        bool bc = false;
        uint32_t bpe = 0;

        switch (format)
        {
            case rhi::TextureFormat::BC1:
            case rhi::TextureFormat::BC4: {
                bc = true;
                bpe = 8;
                break;
            }
            case rhi::TextureFormat::BC3:
            case rhi::TextureFormat::BC5: {
                bc = true;
                bpe = 16;
                break;
            }
        }

        if (bc)
        {
            uint64_t const block_wide = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_width) + 3u) / 4u);
            uint64_t const block_high = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_height) + 3u) / 4u);
            row_bytes = block_wide * bpe;
            row_count = static_cast<uint32_t>(block_high);
        }
        else
        {
            size_t const bpp = 32;
            row_bytes = (mip_width * bpp + 7) / 8;
            row_count = mip_height;
        }
    }
} // namespace ionengine::renderer::rhi