// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/ref_ptr.hpp"
#include "mdl.hpp"

namespace ionengine::asset
{
    class MDLImporter : public core::ref_counted_object
    {
      public:
        virtual ~MDLImporter() = default;

        static auto create() -> core::ref_ptr<MDLImporter>;

        virtual auto loadFromFile(std::filesystem::path const& filePath) -> std::expected<ModelFile, core::error> = 0;
    };
} // namespace ionengine::asset