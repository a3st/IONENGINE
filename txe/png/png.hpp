// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <png.h>
#include "txe/importer.hpp"

namespace ionengine::asset
{
    class PNGImporter : public TXEImporter
    {
      public:
        auto loadFromFile(std::filesystem::path const& filePath, std::string& errors)
            -> std::expected<TextureFile, TXEImportError> override;

        auto loadFromBytes(std::span<uint8_t const> const dataBytes, std::string& errors)
            -> std::expected<TextureFile, TXEImportError> override;

      private:
        std::string* errors;

        auto readTextureData(png_structp png, png_infop pngInfo) -> std::expected<TextureFile, TXEImportError>;
    };
} // namespace ionengine::asset