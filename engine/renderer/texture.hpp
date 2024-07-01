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

    enum class TextureFormat
    {
        RGBA8_UNORM,
        D32_FLOAT
    };

    enum class TextureDimension
    {
        _2D,
        _Cube
    };

    struct TextureData
    {
        TextureFormat format;
        TextureDimension dimension;
        uint32_t width;
        uint32_t height;
        uint32_t numMipLevels;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(format, "format");
            archive.property(dimension, "dimension");
            archive.property(width, "width");
            archive.property(height, "height");
            archive.property(numMipLevels, "mipLevels");
        }
    };

    struct TextureFile
    {
        asset::Header fileHeader;
        TextureData textureData;
        std::vector<std::vector<uint8_t>> textureBuffers;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(fileHeader);
            archive.with<core::serialize::OutputJSON, core::serialize::InputJSON>(textureData);
            archive.property(textureBuffers);
        }
    };

    class TextureAsset : public Asset
    {
        friend class Renderer;

      public:
        TextureAsset(LinkedDevice& device);

        auto loadFromFile(std::filesystem::path const& filePath) -> bool override;

        auto loadFromBytes(std::span<uint8_t const> const dataBytes) -> bool override;

        auto getTextureData() const -> TextureData const&;

        auto getTexture() const -> core::ref_ptr<rhi::Texture>;

        auto dump() -> std::vector<uint8_t>;

      private:
        LinkedDevice* device;
        core::ref_ptr<rhi::Texture> texture;
        TextureData textureData;

        auto create(uint32_t const width, uint32_t const height, TextureUsage const usage) -> void;
    };
} // namespace ionengine

namespace ionengine::core
{
    template <>
    struct SerializableEnum<TextureFormat>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(TextureFormat::RGBA8_UNORM, "RGBA8_UNORM");
            archive.field(TextureFormat::D32_FLOAT, "D32_FLOAT");
        }
    };

    template <>
    struct SerializableEnum<TextureDimension>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(TextureDimension::_2D, "2D");
            archive.field(TextureDimension::_Cube, "Cube");
        }
    };
} // namespace ionengine::core