// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class Texture : public core::ref_counted_object
    {
      public:
        Texture(rhi::Device& device, uint32_t const width, uint32_t const height, rhi::TextureFormat const format,
                rhi::TextureUsageFlags const flags);

        static auto createRenderTarget2D(rhi::Device& device, uint32_t const width, uint32_t const height,
                                         rhi::TextureFormat const format) -> core::ref_ptr<Texture>;

      private:
        core::ref_ptr<rhi::Texture> texture;
    };
} // namespace ionengine

/*
namespace ionengine
{
    class LinkedDevice;

    std::array<uint8_t, 8> constexpr TextureFileType{'T', 'E', 'X', 'F', 'I', 'L', 'E', '\0'};

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
*/