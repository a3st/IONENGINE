// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "texture.hpp"
#include "linked_device.hpp"
#include "precompiled.h"
#include <png.h>

namespace ionengine
{
    TextureAsset::TextureAsset(LinkedDevice& device) : device(&device), textureData({})
    {
    }

    auto TextureAsset::loadFromFile(std::filesystem::path const& filePath) -> bool
    {
        return false;
    }

    auto TextureAsset::loadFromBytes(std::span<uint8_t const> const dataBytes) -> bool
    {
        return false;
    }

    auto TextureAsset::create(uint32_t const width, uint32_t const height, TextureUsage const usage) -> void
    {
        rhi::TextureCreateInfo textureCreateInfo{
            .width = width, .height = height, .depth = 1, .mipLevels = 1, .dimension = rhi::TextureDimension::_2D};

        if (usage == TextureUsage::RenderTarget)
        {
            textureCreateInfo.format = rhi::TextureFormat::RGBA8_UNORM;
            textureCreateInfo.flags =
                (rhi::TextureUsageFlags)rhi::TextureUsage::RenderTarget | rhi::TextureUsage::ShaderResource;

            texture = device->getDevice().createTexture(textureCreateInfo);
        }
        else if (usage == TextureUsage::DepthStencil)
        {
            textureCreateInfo.format = rhi::TextureFormat::D32_FLOAT;
            textureCreateInfo.flags =
                (rhi::TextureUsageFlags)rhi::TextureUsage::DepthStencil | rhi::TextureUsage::ShaderResource;

            texture = device->getDevice().createTexture(textureCreateInfo);
        }
        else
        {
            textureCreateInfo.format = rhi::TextureFormat::RGBA8_UNORM;
            textureCreateInfo.flags = (rhi::TextureUsageFlags)rhi::TextureUsage::ShaderResource;

            texture = device->getDevice().createTexture(textureCreateInfo);
        }
    }

    auto TextureAsset::getTexture() const -> core::ref_ptr<rhi::Texture>
    {
        return texture;
    }

    auto TextureAsset::dump() -> std::vector<uint8_t>
    {
        png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop pngInfo = png_create_info_struct(png);

        struct PNGMemoryIO
        {
            std::basic_stringstream<uint8_t> stream;

            PNGMemoryIO() : stream(std::ios::binary | std::ios::in | std::ios::out)
            {
            }
        };

        std::vector<std::vector<uint8_t>> data;

        device->beginUpload();
        device->readTextureToBytes(texture, data);
        device->endUpload();

        PNGMemoryIO io;

        png_set_write_fn(
            png, &io,
            [](png_structp png, png_bytep data, size_t size) {
                auto io = reinterpret_cast<PNGMemoryIO*>(png_get_io_ptr(png));
                io->stream.write(data, size);
            },
            [](png_structp png) {
                auto io = reinterpret_cast<PNGMemoryIO*>(png_get_io_ptr(png));
                io->stream.flush();
            });

        png_set_IHDR(png, pngInfo, texture->getWidth(), texture->getHeight(), 8, PNG_COLOR_TYPE_RGBA,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        std::vector<uint8_t*> rows(texture->getHeight());
        for (uint32_t const i : std::views::iota(0u, rows.size()))
        {
            rows[i] = data[0].data() + i * texture->getWidth() * 4;
        }

        png_set_rows(png, pngInfo, rows.data());
        png_write_png(png, pngInfo, PNG_TRANSFORM_IDENTITY, nullptr);
        png_write_end(png, pngInfo);
        png_destroy_write_struct(&png, nullptr);

        return std::vector<uint8_t>(std::istreambuf_iterator<uint8_t>(io.stream), {});
    }
} // namespace ionengine