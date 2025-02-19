// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "png.hpp"
#include "precompiled.h"

namespace ionengine::asset
{
    auto PNGImporter::loadFromFile(std::filesystem::path const& filePath, std::string& errors)
        -> std::expected<TextureFile, TXEImportError>
    {
        this->errors = &errors;

        struct PNGStream
        {
            std::basic_ifstream<uint8_t> stream;

            PNGStream(std::filesystem::path const& filePath) : stream(filePath, std::ios::binary)
            {
            }
        };

        PNGStream pngStream(filePath);

        png_structp png = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop pngInfo = ::png_create_info_struct(png);

        ::png_set_read_fn(png, &pngStream, [](png_structp png, png_bytep data, size_t size) {
            auto instance = reinterpret_cast<PNGStream*>(png_get_io_ptr(png));
            instance->stream.read(data, size);
        });

        auto readResult = this->readTextureData(png, pngInfo);

        ::png_destroy_read_struct(&png, &pngInfo, nullptr);
        return readResult;
    }

    auto PNGImporter::loadFromBytes(std::span<uint8_t const> const dataBytes, std::string& errors)
        -> std::expected<TextureFile, TXEImportError>
    {
        this->errors = &errors;

        struct PNGStream
        {
            std::basic_ispanstream<uint8_t> stream;

            PNGStream(std::span<uint8_t const> const dataBytes)
                : stream(std::span<uint8_t>(const_cast<uint8_t*>(dataBytes.data()), dataBytes.size()), std::ios::binary)
            {
            }
        };

        PNGStream pngStream(dataBytes);

        png_structp png = ::png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop pngInfo = ::png_create_info_struct(png);

        ::png_set_read_fn(png, &pngStream, [](png_structp png, png_bytep data, size_t size) {
            auto instance = reinterpret_cast<PNGStream*>(png_get_io_ptr(png));
            instance->stream.read(data, size);
        });

        auto readResult = this->readTextureData(png, pngInfo);

        ::png_destroy_read_struct(&png, &pngInfo, nullptr);
        return readResult;
    }

    auto PNGImporter::readTextureData(png_structp png, png_infop pngInfo) -> std::expected<TextureFile, TXEImportError>
    {
        ::png_read_info(png, pngInfo);

        txe::TextureData textureData{
            .format = txe::TextureFormat::RGBA8_UNORM, .dimension = txe::TextureDimension::_2D, .mipLevels = {0}};
        std::basic_stringstream<uint8_t> textureBlob;

        int32_t bitDepth;
        int32_t colorType;
        ::png_get_IHDR(png, pngInfo, &textureData.width, &textureData.height, &bitDepth, &colorType, nullptr, nullptr,
                       nullptr);

        size_t const rowBytes = png_get_rowbytes(png, pngInfo);
        std::vector<uint8_t> rowData(rowBytes);

        for (uint32_t const _ : std::views::iota(0u, textureData.height))
        {
            png_read_row(png, rowData.data(), nullptr);

            uint64_t offset = 0;
            for (uint32_t const _ : std::views::iota(0u, textureData.width))
            {
                uint8_t const r = rowData[offset++];
                uint8_t const g = rowData[offset++];
                uint8_t const b = rowData[offset++];

                uint8_t a;
                if (colorType == PNG_COLOR_TYPE_RGBA)
                {
                    a = rowData[offset++];
                }
                else
                {
                    a = 255;
                }

                textureBlob.write(&r, 1);
                textureBlob.write(&g, 1);
                textureBlob.write(&b, 1);
                textureBlob.write(&a, 1);
            }
        }

        txe::BufferData const bufferData{.offset = 0, .size = static_cast<size_t>(textureBlob.tellp())};
        textureData.buffers.emplace_back(std::move(bufferData));

        return TextureFile{.magic = txe::Magic,
                           .textureData = std::move(textureData),
                           .blob = {std::istreambuf_iterator<uint8_t>(textureBlob.rdbuf()), {}}};
    }
} // namespace ionengine::asset