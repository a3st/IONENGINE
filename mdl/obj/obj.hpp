// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/vector.hpp"
#include "mdl/importer.hpp"

namespace ionengine::asset
{
    class OBJImporter : public MDLImporter
    {
      public:
        auto loadFromFile(std::filesystem::path const& filePath) -> std::expected<ModelFile, core::error> override;

      private:
        struct Vertex
        {
            core::Vec3f position;
            core::Vec3f normal;
            core::Vec2f uv;

            auto operator==(Vertex const& other) const -> bool
            {
                return std::make_tuple(position, normal, uv) == std::make_tuple(other.position, other.normal, other.uv);
            }
        };

        struct VertexHasher
        {
            auto operator()(const Vertex& other) const -> std::size_t
            {
                return std::hash<core::Vec3f>()(other.position) ^ std::hash<core::Vec3f>()(other.normal) ^
                       std::hash<core::Vec2f>()(other.uv);
            }
        };
    };
} // namespace ionengine::asset