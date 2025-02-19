// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine::asset
{
    namespace txe
    {
        std::array<uint8_t, 4> constexpr Magic{'T', 'X', '1', '0'};

        enum class TextureFormat
        {
            RGBA8_UNORM
        };

        enum class TextureDimension
        {
            _2D,
            Cube
        };

        struct BufferData
        {
            uint64_t offset;
            size_t size;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(offset, "offset");
                archive.property(size, "sizeInBytes");
            }
        };

        struct TextureData
        {
            TextureFormat format;
            TextureDimension dimension;
            uint32_t width;
            uint32_t height;
            std::vector<uint32_t> mipLevels;
            std::vector<BufferData> buffers;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(format, "format");
                archive.property(dimension, "dimension");
                archive.property(width, "width");
                archive.property(height, "height");
                archive.property(mipLevels, "mipLevels");
                archive.property(buffers, "buffers");
            }
        };
    } // namespace txe

    struct TextureFile
    {
        std::array<uint8_t, txe::Magic.size()> magic;
        txe::TextureData textureData;
        std::vector<uint8_t> blob;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.template with<core::serialize_ojson, core::serialize_ijson>(textureData);
            archive.property(blob);
        }
    };
} // namespace ionengine::asset

namespace ionengine::core
{
    template <>
    struct serializable_enum<asset::txe::TextureFormat>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(asset::txe::TextureFormat::RGBA8_UNORM, "RGBA8_UNORM");
        }
    };

    template <>
    struct serializable_enum<asset::txe::TextureDimension>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(asset::txe::TextureDimension::_2D, "2D");
            archive.field(asset::txe::TextureDimension::Cube, "Cube");
        }
    };
} // namespace ionengine::core