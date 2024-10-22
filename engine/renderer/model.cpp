// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "model.hpp"
#include "precompiled.h"

namespace ionengine
{
    Model::Model(rhi::Device& device, rhi::CopyContext& copyContext, asset::ModelFile const& modelFile)
    {
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        {
            asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[modelFile.modelData.buffer];

            rhi::BufferCreateInfo bufferCreateInfo{
                .size = bufferData.size,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};
            vertexBuffer = device.createBuffer(bufferCreateInfo);

            copyContext
                .writeBuffer(vertexBuffer,
                             std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size))
                .wait();
        }

        for (auto const& surfaceData : modelFile.modelData.surfaces)
        {
            asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[surfaceData.buffer];

            rhi::BufferCreateInfo bufferCreateInfo{
                .size = bufferData.size,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Index | rhi::BufferUsage::CopyDest)};
            core::ref_ptr<rhi::Buffer> indexBuffer = device.createBuffer(bufferCreateInfo);

            copyContext
                .writeBuffer(indexBuffer,
                             std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size))
                .wait();

            auto surface = core::make_ref<Surface>(vertexBuffer, indexBuffer, surfaceData.indexCount);
            surfaces.emplace_back(surface);
        }
    }

    auto Model::setMaterial(uint32_t const index, core::ref_ptr<Material> material) -> void
    {
        materials[index] = material;
    }

    auto Model::getSurfaces() const -> std::span<core::ref_ptr<Surface> const>
    {
        return surfaces;
    }

    auto Model::getMaterials() const -> std::span<core::ref_ptr<Material> const>
    {
        return materials;
    }

    /*auto Model::addTo(RenderQueue& opaque, RenderQueue& translucent, uint16_t const layer) -> void
    {
        for (size_t const i : std::views::iota(0u, surfaces.size()))
        {
            switch (materials[i]->blend)
            {
                case MaterialBlend::Opaque: {
                    opaque.push(surfaces[i], materials[i], layer);
                    break;
                }
                case MaterialBlend::Translucent: {
                    translucent.push(surfaces[i], materials[i], layer);
                    break;
                }
                default: {
                    throw core::runtime_error("");
                }
            }
        }
    }*/
} // namespace ionengine