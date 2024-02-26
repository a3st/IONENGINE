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

        struct Data
        {
            std::vector<Mesh> meshes;
        };

        virtual auto load(std::span<uint8_t const> const data) -> bool = 0;

        virtual auto get() -> Data const& = 0;
    };
} // namespace ionengine