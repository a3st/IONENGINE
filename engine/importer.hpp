// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine
{
    class ModelImporter
    {
      public:
        struct Primitive
        {
            std::vector<uint8_t> vertices;
            std::vector<uint8_t> indices;
            uint32_t index_count;
        };

        struct Mesh
        {
            uint32_t material_index;
            std::vector<Primitive> primitives;
        };

        struct ModelData
        {
            std::vector<Mesh> meshes;
        };

        virtual auto loadFromMemory(std::span<uint8_t const> const data) -> bool = 0;

        ModelData modelData;
    };
} // namespace ionengine