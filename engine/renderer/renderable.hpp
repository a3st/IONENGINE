// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    struct Surface : public core::ref_counted_object
    {
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        core::ref_ptr<rhi::Buffer> indexBuffer;
        uint32_t numIndices;
        uint64_t hash;
    };

    enum class RenderableType
    {
        Quad,
        Surface,
        Compute
    };

    struct RenderableData
    {
        RenderableType renderableType;
        core::ref_ptr<Surface> surface;

        core::ref_ptr<rhi::Shader> shader;
        rhi::RasterizerStageInfo rasterizerStage;
        rhi::BlendColorInfo blendColor;
        std::optional<rhi::DepthStencilStageInfo> depthStencil;

        std::unordered_map<uint32_t, rhi::TextureBindData> boundedTextures;
        std::unordered_map<uint32_t, rhi::BufferBindData> boundedBuffers;

        uint64_t zIndex;
        uint64_t shaderIndex;
    };
} // namespace ionengine