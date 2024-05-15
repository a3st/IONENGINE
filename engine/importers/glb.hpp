// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/importer.hpp"

namespace ionengine
{
    class GLBImporter : public ModelImporter
    {
      public:
        auto loadFromMemory(std::span<uint8_t const> const data) -> bool override;
    };
} // namespace ionengine