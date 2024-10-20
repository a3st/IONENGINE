// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "model.hpp"
#include "precompiled.h"

namespace ionengine
{
    Surface::Surface(core::ref_ptr<rhi::Buffer> vertexBuffer, core::ref_ptr<rhi::Buffer> indexBuffer,
                     uint32_t const indexCount)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), indexCount(indexCount)
    {
    }

    auto Surface::draw(rhi::GraphicsContext& context) -> void
    {
        context.bindVertexBuffer(vertexBuffer, 0, vertexBuffer->getSize());
        context.bindIndexBuffer(indexBuffer, 0, indexBuffer->getSize(), rhi::IndexFormat::Uint32);
        context.drawIndexed(indexCount, 1);
    }

    Model::Model(rhi::Device& device, rhi::CopyContext& copyContext, ModelFile const& modelFile)
        : copyContext(&copyContext)
    {
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        {
            mdl::BufferData const& bufferData = modelFile.modelData.buffers[modelFile.modelData.buffer];

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
            mdl::BufferData const& bufferData = modelFile.modelData.buffers[surfaceData.buffer];

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

    auto Model::render(RenderingData& renderingData) -> void
    {
        for (size_t const i : std::views::iota(0u, surfaces.size()))
        {
            auto& material = materials[i];
            material->update(*copyContext);

            ObjectData objectData{
                .surface = surfaces[i], .shader = material->getShader(false), .materialBuffer = material->getBuffer()};
            renderingData.drawResults.objects.emplace_back(std::move(objectData));
        }
    }
} // namespace ionengine