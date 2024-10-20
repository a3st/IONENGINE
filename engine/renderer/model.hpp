// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "material.hpp"
#include "mdl/mdl.hpp"
#include "rhi/rhi.hpp"
#include "shader.hpp"

namespace ionengine
{
    class Surface : public core::ref_counted_object
    {
      public:
        Surface(core::ref_ptr<rhi::Buffer> vertexBuffer, core::ref_ptr<rhi::Buffer> indexBuffer,
                uint32_t const indexCount);

        auto draw(rhi::GraphicsContext& context) -> void;

      private:
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        core::ref_ptr<rhi::Buffer> indexBuffer;
        uint32_t indexCount;
    };

    struct ObjectData
    {
        core::ref_ptr<Surface> surface;
        core::ref_ptr<ShaderVariant> shader;
        core::ref_ptr<rhi::Buffer> materialBuffer;
    };

    struct DrawResults
    {
        std::vector<size_t> visibleObjects;
        std::vector<ObjectData> objects;
        // std::vector<shadersys::common::LightingData> visibleLights;
    };

    struct RenderingData
    {
        DrawResults drawResults;
        core::weak_ptr<rhi::Texture> backBuffer;
    };

    class Model : public core::ref_counted_object
    {
      public:
        Model(rhi::Device& device, rhi::CopyContext& copyContext, ModelFile const& modelFile);

        auto setMaterial(uint32_t const index, core::ref_ptr<Material> material) -> void;

        auto render(RenderingData& renderingData) -> void;

      private:
        rhi::CopyContext* copyContext;

        std::vector<core::ref_ptr<Surface>> surfaces;
        std::vector<core::ref_ptr<Material>> materials;
    };
} // namespace ionengine