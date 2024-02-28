#pragma once

#include "extensions/importer.hpp"
#include "linked_device.hpp"
#include "material.hpp"

namespace ionengine
{
    struct Primitive
    {
        core::ref_ptr<rhi::Buffer> vertices;
        core::ref_ptr<rhi::Buffer> indices;
        uint32_t index_count;
    };

    struct Mesh
    {
        core::ref_ptr<Material> material;
        std::vector<Primitive> primitives;
    };

    class Model : public Asset
    {
      public:
        Model(LinkedDevice& device);

        auto load_from_memory(std::span<uint8_t const> const data, ModelImporter& importer) -> bool;

      private:
        LinkedDevice* device;
        std::vector<Mesh> meshes;
    };
} // namespace ionengine