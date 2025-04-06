// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "mesh.hpp"
#include "precompiled.h"
#include "upload_manager.hpp"

namespace ionengine
{
    Mesh::Mesh(rhi::RHI& RHI, UploadManager& uploadManager, asset::ModelFile const& modelFile)
    {
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        {
            asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[modelFile.modelData.buffer];

            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = bufferData.size,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};
            vertexBuffer = RHI.createBuffer(bufferCreateInfo);

            UploadBufferInfo const uploadBufferInfo{
                .buffer = vertexBuffer,
                .offset = 0,
                .dataBytes = std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size)};
            uploadManager.uploadBuffer(uploadBufferInfo, [this]() -> void {});
        }

        for (auto const& surfaceData : modelFile.modelData.surfaces)
        {
            asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[surfaceData.buffer];

            auto surface = core::make_ref<Surface>(RHI, vertexBuffer, bufferData.size, surfaceData.indexCount);

            UploadBufferInfo const uploadBufferInfo{
                .buffer = surface->getIndexBuffer(),
                .offset = 0,
                .dataBytes = std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size)};
            uploadManager.uploadBuffer(uploadBufferInfo, [this]() -> void {});

            surfaces.emplace_back(std::move(surface));
            materials.emplace_back(Material::baseSurfaceMaterial);
        }
    }

    auto Mesh::getSurfaces() const -> std::vector<core::ref_ptr<Surface>> const&
    {
        return surfaces;
    }

    auto Mesh::getMaterials() const -> std::vector<core::ref_ptr<Material>> const&
    {
        return materials;
    }

    auto Mesh::setMaterial(uint32_t const index, core::ref_ptr<Material> const& material) -> void
    {
        materials[index] = material;
    }
} // namespace ionengine