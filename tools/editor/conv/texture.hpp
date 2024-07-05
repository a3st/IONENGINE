// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/renderer/texture.hpp"

namespace ionengine::tools::editor
{
    class TextureImport
    {
      public:
        TextureImport();

        auto loadFromFile(std::filesystem::path const& filePath) -> std::optional<TextureFile>;

        auto loadFromBytes(std::span<uint8_t const> const dataBytes) -> std::optional<TextureFile>;
    };
} // namespace ionengine::tools::editor