// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "mdl/mdl.hpp"
#include "queue.hpp"

namespace ionengine
{
    class Model : public core::ref_counted_object
    {
      public:
        Model(rhi::Device& device, rhi::CopyContext& copyContext, asset::ModelFile const& modelFile);

        auto setMaterial(uint32_t const index, core::ref_ptr<Material> material) -> void;

        auto getSurfaces() const -> std::span<core::ref_ptr<Surface> const>;

        auto getMaterials() const -> std::span<core::ref_ptr<Material> const>;

      private:
        std::vector<core::ref_ptr<Surface>> surfaces;
        std::vector<core::ref_ptr<Material>> materials;
    };
} // namespace ionengine