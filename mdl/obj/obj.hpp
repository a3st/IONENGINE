// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/vector.hpp"
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
            math::Vec3f position;
            math::Vec3f normal;
            math::Vec2f uv;

            auto operator==(Vertex const& other) const -> bool
            {
                return std::make_tuple(position, normal, uv) == std::make_tuple(other.position, other.normal, other.uv);
            }
        };

        struct VertexHasher
        {
            auto operator()(const Vertex& other) const -> std::size_t
            {
                return std::hash<math::Vec3f>()(other.position) ^ std::hash<math::Vec3f>()(other.normal) ^
                       std::hash<math::Vec2f>()(other.uv);
            }
        };
    };
} // namespace ionengine::asset