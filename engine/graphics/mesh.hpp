// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "material.hpp"
#include "mdl/mdl.hpp"
#include "upload_manager.hpp"

namespace ionengine
{
    struct Surface
    {
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        core::ref_ptr<rhi::Buffer> indexBuffer;
        uint32_t indexCount;

        Surface(rhi::RHI& RHI, core::ref_ptr<rhi::Buffer> vertexBuffer, size_t const indexSize,
                uint32_t const indexCount)
            : vertexBuffer(vertexBuffer), indexCount(indexCount)
        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = indexSize,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Index | rhi::BufferUsage::CopyDest)};
            indexBuffer = RHI.createBuffer(bufferCreateInfo);
        }

        Surface(rhi::RHI& RHI, size_t const vertexSize, size_t const indexSize, uint32_t const indexCount)
            : indexCount(indexCount)
        {
            {
                rhi::BufferCreateInfo const bufferCreateInfo{
                    .size = vertexSize,
                    .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};
                vertexBuffer = RHI.createBuffer(bufferCreateInfo);
            }

            {
                rhi::BufferCreateInfo const bufferCreateInfo{
                    .size = indexSize,
                    .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Index | rhi::BufferUsage::CopyDest)};
                indexBuffer = RHI.createBuffer(bufferCreateInfo);
            }
        }

        auto draw(rhi::GraphicsContext* context) const -> void
        {
            context->bindVertexBuffer(vertexBuffer, 0, vertexBuffer->getSize());
            context->bindIndexBuffer(indexBuffer, 0, indexBuffer->getSize(), rhi::IndexFormat::Uint32);
            context->drawIndexed(indexCount, 1);
        }
    };

    class Mesh : public core::ref_counted_object
    {
      public:
        Mesh(rhi::RHI& RHI, internal::UploadManager* uploadManager, asset::ModelFile const& modelFile);

        auto getSurfaces() const -> std::span<Surface const>;

        auto getMaterials() const -> std::span<core::ref_ptr<Material> const>;

        auto setMaterial(uint32_t const index, core::ref_ptr<Material> const& material) -> void;

      private:
        std::vector<Surface> surfaces;
        std::vector<core::ref_ptr<Material>> materials;
    };
} // namespace ionengine