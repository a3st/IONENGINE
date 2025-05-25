// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "mesh.hpp"
#include "precompiled.h"
#include "upload_manager.hpp"
#include <xxhash.h>

namespace ionengine
{
    Mesh::Mesh(rhi::RHI& RHI, UploadManager& uploadManager,
               std::unordered_map<uint64_t, core::ref_ptr<Surface>>& surfacesCache, asset::ModelFile const& modelFile)
    {
        XXH64_state_t* hasher = ::XXH64_createState();
        core::ref_ptr<rhi::Buffer> vertexBuffer;

        for (auto const& surfaceData : modelFile.modelData.surfaces)
        {
            core::ref_ptr<Surface> surface;

            // Try to find Surface from Cache
            ::XXH64_reset(hasher, 0);

            {
                asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[modelFile.modelData.buffer];
                ::XXH64_update(hasher, modelFile.blob.data() + bufferData.offset, bufferData.size);
            }

            {
                asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[surfaceData.buffer];
                ::XXH64_update(hasher, modelFile.blob.data() + bufferData.offset, bufferData.size);

                uint64_t const surfaceHash = ::XXH64_digest(hasher);
                auto surfaceIt = surfacesCache.find(surfaceHash);

                if (surfaceIt != surfacesCache.end())
                {
                    surface = surfaceIt->second;
                }
            }

            // If Surface is exists, then continue
            if (surface)
            {
                continue;
            }
            else
            {
                if (!vertexBuffer)
                {
                    asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[modelFile.modelData.buffer];

                    rhi::BufferCreateInfo const bufferCreateInfo{
                        .size = bufferData.size,
                        .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};
                    vertexBuffer = RHI.createBuffer(bufferCreateInfo);

                    UploadBufferInfo const uploadBufferInfo{
                        .buffer = vertexBuffer,
                        .offset = 0,
                        .dataBytes =
                            std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size)};
                    uploadManager.uploadBuffer(uploadBufferInfo);
                }

                asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[surfaceData.buffer];

                rhi::BufferCreateInfo const bufferCreateInfo{
                    .size = bufferData.size,
                    .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Index | rhi::BufferUsage::CopyDest)};
                core::ref_ptr<rhi::Buffer> indexBuffer = RHI.createBuffer(bufferCreateInfo);

                surface = core::make_ref<Surface>(vertexBuffer, indexBuffer, surfaceData.indexCount);

                UploadBufferInfo const uploadBufferInfo{
                    .buffer = indexBuffer,
                    .offset = 0,
                    .dataBytes = std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size)};
                uploadManager.uploadBuffer(uploadBufferInfo);

                surfaces.emplace_back(surface);
                materials.emplace_back(Material::baseSurfaceMaterial);
            }
        }

        ::XXH64_freeState(hasher);
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