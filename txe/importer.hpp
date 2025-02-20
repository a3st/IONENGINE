// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/ref_ptr.hpp"
#include "txe.hpp"

namespace ionengine::asset
{
    class TXEImporter : public core::ref_counted_object
    {
      public:
        virtual ~TXEImporter() = default;

        virtual auto loadFromFile(std::filesystem::path const& filePath) -> std::expected<TextureFile, core::error> = 0;
    };
} // namespace ionengine::asset