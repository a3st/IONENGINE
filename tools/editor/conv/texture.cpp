// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "conv/texture.hpp"
#include "core/serializable.hpp"
#include "precompiled.h"
#include <png.h>

namespace ionengine::tools::editor
{

    TextureImport::TextureImport()
    {
    }

    auto TextureImport::loadFromFile(std::filesystem::path const& filePath) -> std::optional<TextureFile>
    {
        if (filePath.extension().compare(".png") == 0)
        {
            png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
            png_infop pngInfo = png_create_info_struct(png);

            struct PNGMemoryIO
            {
                std::basic_ifstream<uint8_t> stream;

                PNGMemoryIO(std::filesystem::path const& filePath) : stream(filePath, std::ios::binary)
                {
                }
            };

            PNGMemoryIO io(filePath);

            png_set_read_fn(png, &io, [](png_structp png, png_bytep data, size_t size) {
                auto io = reinterpret_cast<PNGMemoryIO*>(png_get_io_ptr(png));
                io->stream.read(data, size);
            });

            png_read_info(png, pngInfo);

            TextureData textureData{
                .format = TextureFormat::RGBA8_UNORM, .dimension = TextureDimension::_2D, .mipLevels = 1};
            int32_t bitDepth;
            int32_t colorType;
            png_get_IHDR(png, pngInfo, &textureData.width, &textureData.height, &bitDepth, &colorType, nullptr, nullptr,
                         nullptr);

            std::vector<std::vector<uint8_t>> buffers(1);
            switch (colorType)
            {
                case PNG_COLOR_TYPE_RGB: {
                    size_t const rowBytes = png_get_rowbytes(png, pngInfo);
                    std::vector<uint8_t> rowData(rowBytes);

                    std::vector<uint8_t> mipData(textureData.width * textureData.height * 4);
                    std::basic_spanstream<uint8_t> stream(std::span<uint8_t>(mipData.data(), mipData.size()),
                                                          std::ios::in);

                    for (size_t const i : std::views::iota(0u, textureData.height))
                    {
                        png_read_row(png, rowData.data(), nullptr);

                        uint64_t offset = 0;
                        for (size_t const j : std::views::iota(0u, textureData.width))
                        {
                            uint8_t const r = rowData[offset++];
                            uint8_t const g = rowData[offset++];
                            uint8_t const b = rowData[offset++];
                            uint8_t const a = 255;

                            stream.write(&r, 1);
                            stream.write(&g, 1);
                            stream.write(&b, 1);
                            stream.write(&a, 1);
                        }
                    }

                    buffers.emplace_back(std::move(mipData));
                    break;
                }
                case PNG_COLOR_TYPE_RGB_ALPHA: {
                    size_t const rowBytes = png_get_rowbytes(png, pngInfo);
                    std::vector<uint8_t> rowData(rowBytes);

                    std::vector<uint8_t> mipData(textureData.width * textureData.height * 4);
                    std::basic_spanstream<uint8_t> stream(std::span<uint8_t>(mipData.data(), mipData.size()),
                                                          std::ios::in);

                    for (size_t const i : std::views::iota(0u, textureData.height))
                    {
                        png_read_row(png, rowData.data(), nullptr);

                        uint64_t offset = 0;
                        for (size_t const j : std::views::iota(0u, textureData.width))
                        {
                            uint8_t const r = rowData[offset++];
                            uint8_t const g = rowData[offset++];
                            uint8_t const b = rowData[offset++];
                            uint8_t const a = rowData[offset++];

                            stream.write(&r, 1);
                            stream.write(&g, 1);
                            stream.write(&b, 1);
                            stream.write(&a, 1);
                        }
                    }

                    buffers.emplace_back(std::move(mipData));
                    break;
                }
            }

            png_destroy_read_struct(&png, &pngInfo, nullptr);

            TextureFile textureFile{.fileHeader = asset::Header{.magic = asset::Magic, .fileType = TextureFileType},
                                    .textureData = std::move(textureData),
                                    .buffers = std::move(buffers)};

            auto buffer = core::saveToBytes<TextureFile, core::serialize::OutputArchive>(textureFile).value();
            return core::loadFromBytes<TextureFile, core::serialize::InputArchive>(buffer);
        }
        return std::nullopt;
    }

    auto TextureImport::loadFromBytes(std::span<uint8_t const> const dataBytes) -> std::optional<TextureFile>
    {
        return std::nullopt;
    }
} // namespace ionengine::tools::editor