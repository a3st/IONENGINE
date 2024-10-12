// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "model.hpp"
#include "precompiled.h"

namespace ionengine
{
    Surface::Surface(core::ref_ptr<rhi::Buffer> vertexBuffer, core::ref_ptr<rhi::Buffer> indexBuffer,
                     uint32_t const indexCount, uint32_t const material)
        : vertexBuffer(vertexBuffer), indexBuffer(indexBuffer), indexCount(indexCount), material(material)
    {
    }

    auto Surface::draw(rhi::GraphicsContext& context) -> void
    {
        shader->setActive(context);

        context.drawIndexed(indexCount, 1);
    }

    Model::Model(rhi::Device& device, rhi::CopyContext& copyContext, ModelFile const& modelFile)
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

            auto surface =
                core::make_ref<Surface>(vertexBuffer, indexBuffer, surfaceData.indexCount, surfaceData.material);
            surfaces.emplace_back(surface);
        }
    }
} // namespace ionengine