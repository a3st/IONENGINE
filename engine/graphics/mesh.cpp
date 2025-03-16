// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "mesh.hpp"
#include "precompiled.h"

namespace ionengine
{
    Mesh::Mesh(rhi::RHI& RHI, internal::UploadManager* uploadManager, asset::ModelFile const& modelFile)
    {
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        {
            asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[modelFile.modelData.buffer];

            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = bufferData.size,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};
            vertexBuffer = RHI.createBuffer(bufferCreateInfo);

            internal::UploadBufferInfo const uploadBufferInfo{
                .buffer = vertexBuffer,
                .offset = 0,
                .dataBytes = std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size)};
            uploadManager->uploadBuffer(uploadBufferInfo, [this]() -> void { countUploaded++; });
        }

        for (auto const& surfaceData : modelFile.modelData.surfaces)
        {
            asset::mdl::BufferData const& bufferData = modelFile.modelData.buffers[surfaceData.buffer];

            Surface surface(RHI, vertexBuffer, bufferData.size, surfaceData.indexCount);

            internal::UploadBufferInfo const uploadBufferInfo{
                .buffer = surface.indexBuffer,
                .offset = 0,
                .dataBytes = std::span<uint8_t const>(modelFile.blob.data() + bufferData.offset, bufferData.size)};
            uploadManager->uploadBuffer(uploadBufferInfo, [this]() -> void { countUploaded++; });

            surfaces.emplace_back(std::move(surface));
        }
    }

    auto Mesh::isUploaded() const -> bool
    {
        return surfaces.size() + 1 == countUploaded;
    }

    auto Mesh::getSurfaces() const -> std::span<Surface const>
    {
        return surfaces;
    }
} // namespace ionengine