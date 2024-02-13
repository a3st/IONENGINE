// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "ktx2_image.hpp"
#include "precompiled.h"

namespace ionengine::ktx2
{
    Ktx2Image::Ktx2Image(std::span<uint8_t const> const data_bytes)
    {
        std::basic_ispanstream<uint8_t> stream(
            std::span<uint8_t>(const_cast<uint8_t*>(data_bytes.data()), data_bytes.size()), std::ios::binary);

        auto header = Header{};
        stream.read((uint8_t*)&header, sizeof(Header));

        if (std::memcmp(header.magic.data(), KTX_MAGIC.data(), sizeof(KTX_MAGIC)) != 0)
        {
            throw core::Exception("Trying to parse a corrupted ktx2 image");
        }

        switch (header.supercompression_scheme)
        {
            case 1: {
                throw core::Exception("BasisLZ is not supported by ktx2 image loader");
            }
            break;
            case 2: {
                throw core::Exception("Zstandard is not supported by ktx2 image loader");
            }
            break;
            case 3: {
                throw core::Exception("ZLIB is not supported by ktx2 image loader");
            }
            break;
        }

        stream.seekg(4 * sizeof(uint32_t) + 2 * sizeof(uint64_t), std::ios::cur);

        std::vector<MipLevelData> mip_level_data(header.level_count);
        for (uint32_t i = 0; i < header.level_count; ++i)
        {
            stream.read((uint8_t*)&mip_level_data[i], sizeof(MipLevelData));
        }

        mip_data.resize(header.level_count);
        for (uint32_t i = 0; i < header.level_count; ++i)
        {
            stream.seekg(mip_level_data[i].byte_offset, std::ios::beg);

            switch (header.supercompression_scheme)
            {
                case 0: {
                    mip_data[i].resize(mip_level_data[i].byte_length);
                    stream.read((uint8_t*)mip_data[i].data(), mip_level_data[i].byte_length);
                }
                break;
                default: {
                    throw core::Exception("Compression file is not supported by ktx2 image loader");
                }
                break;
            }
        }

        ImageFormat image_format = ImageFormat::Unknown;
        switch (header.vk_format)
        {
            case 138: {
                image_format = ImageFormat::BC3;
            }
            break;
        }

        image_data = ImageData{.format = image_format,
                               .pixel_width = header.pixel_width,
                               .pixel_height = header.pixel_height,
                               .pixel_depth = header.pixel_depth,
                               .layer_count = header.layer_count,
                               .face_count = header.face_count,
                               .level_count = header.level_count};
    }
} // namespace ionengine::ktx2