
#include "model.hpp"
#include "precompiled.h"

namespace ionengine
{
    Model::Model(RenderContext& context) : context(&context)
    {
    }

    auto Model::load_from_memory(std::span<uint8_t const> const data, ModelImporter& importer) -> bool
    {
        auto result = importer.load(data);

        if (!result)
        {
            return false;
        }

        for (auto const& mesh : importer.get().meshes)
        {

            for (auto const& primitive : mesh.primitives)
            {
                auto vertex_buffer = core::make_ref<rhi::Buffer>(primitive.vertices.size(), 0,
                                                                 rhi::BufferUsageFlags(rhi::BufferUsage::Vertex));
                auto index_buffer = core::make_ref<rhi::Buffer>(primitive.indices.size(), 0,
                                                                rhi::BufferUsageFlags(rhi::BufferUsage::Index));

                
            }
        }

        return true;
    }
} // namespace ionengine