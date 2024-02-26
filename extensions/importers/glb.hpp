#pragma once

#include "extensions/importer.hpp"

namespace ionengine
{
    class GLBImporter : public ModelImporter
    {
      public:
        auto load(std::span<uint8_t const> const data) -> bool override;

        auto get() -> Data const& override
        {
            return data;
        }

      private:
        Data data;
    };
} // namespace ionengine