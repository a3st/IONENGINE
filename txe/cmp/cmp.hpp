// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "txe/importer.hpp"

namespace ionengine::asset
{
    class CMPImporter : public TXEImporter
    {
      public:
        CMPImporter(bool generateMipMaps);

        auto loadFromFile(std::filesystem::path const& filePath) -> std::expected<TextureFile, core::error> override;

      private:
        bool generateMipMaps;
    };
} // namespace ionengine::asset