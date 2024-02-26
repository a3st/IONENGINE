#pragma once

#include "extensions/importer.hpp"
#include "material.hpp"
#include "render_context.hpp"

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
        Model(RenderContext& context);

        auto load_from_memory(std::span<uint8_t const> const data, ModelImporter& importer)
            -> bool;

      private:
        RenderContext* context;
        std::vector<Mesh> meshes;
    };
} // namespace ionengine