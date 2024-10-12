// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "mdl/mdl.hpp"
#include "rhi/rhi.hpp"
#include "shader.hpp"
#include "material.hpp"

namespace ionengine
{
    class Surface : public core::ref_counted_object
    {
      public:
        Surface(core::ref_ptr<rhi::Buffer> vertexBuffer, core::ref_ptr<rhi::Buffer> indexBuffer,
                uint32_t const indexCount, uint32_t const material);

        auto draw(rhi::GraphicsContext& context) -> void;

      private:
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        core::ref_ptr<rhi::Buffer> indexBuffer;
        uint32_t indexCount;
        core::ref_ptr<Shader> shader;
        uint32_t material;
    };

    class Model : public core::ref_counted_object
    {
      public:
        Model(rhi::Device& device, rhi::CopyContext& copyContext, ModelFile const& modelFile);

      private:
        std::vector<core::ref_ptr<Surface>> surfaces;
        std::vector<core::ref_ptr<Material>> materials;
    };
} // namespace ionengine