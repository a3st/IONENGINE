// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "txe.hpp"

namespace ionengine::asset
{
    enum class TXEImportError
    {
        Parse
    };

    class TXEImporter : public core::ref_counted_object
    {
      public:
        virtual ~TXEImporter() = default;

        virtual auto loadFromFile(std::filesystem::path const& filePath, std::string& errors)
            -> std::expected<TextureFile, TXEImportError> = 0;

        virtual auto loadFromBytes(std::span<uint8_t const> const dataBytes, std::string& errors)
            -> std::expected<TextureFile, TXEImportError> = 0;
    };
} // namespace ionengine::asset