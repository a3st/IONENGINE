// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/exception.hpp"

namespace ionengine::ktx2
{
    inline std::array<uint8_t, 12> const KTX_MAGIC{0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32,
                                                   0x30, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};

    enum class ImageFormat
    {
        Unknown,
        BC3
    };

    struct Header
    {
        std::array<uint8_t, 12> magic;
        uint32_t vk_format;
        uint32_t type_size;
        uint32_t pixel_width;
        uint32_t pixel_height;
        uint32_t pixel_depth;
        uint32_t layer_count;
        uint32_t face_count;
        uint32_t level_count;
        uint32_t supercompression_scheme;
    };

    struct ImageData
    {
        ImageFormat format;
        uint32_t pixel_width;
        uint32_t pixel_height;
        uint32_t pixel_depth;
        uint32_t layer_count;
        uint32_t face_count;
        uint32_t level_count;
    };

    struct MipLevelData
    {
        uint64_t byte_offset;
        uint64_t byte_length;
        uint64_t uncompressed_byte_length;
    };

    class Ktx2Image
    {
      public:
        Ktx2Image(std::span<uint8_t const> const data_bytes);

        auto get_image_data() const -> ImageData const&
        {
            return image_data;
        }

        auto get_mip_data(uint32_t const index) const -> std::span<uint8_t const>
        {
            return mip_data[index];
        }

      private:
        ImageData image_data;
        std::vector<std::vector<uint8_t>> mip_data;
    };
} // namespace ionengine::ktx2