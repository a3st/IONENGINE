// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "material.hpp"
#include "surface.hpp"
#include "mdl/mdl.hpp"
#include "upload_manager.hpp"

namespace ionengine
{
    class Mesh : public core::ref_counted_object
    {
      public:
        Mesh(rhi::RHI& RHI, UploadManager* uploadManager, asset::ModelFile const& modelFile);

        auto getSurfaces() const -> std::vector<core::ref_ptr<Surface>> const&;

        auto getMaterials() const -> std::vector<core::ref_ptr<Material>> const&;

        auto setMaterial(uint32_t const index, core::ref_ptr<Material> const& material) -> void;

      private:
        std::vector<core::ref_ptr<Surface>> surfaces;
        std::vector<core::ref_ptr<Material>> materials;
    };
} // namespace ionengine