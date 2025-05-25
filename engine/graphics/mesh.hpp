// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "entity.hpp"
#include "material.hpp"
#include "mdl/mdl.hpp"
#include "surface.hpp"

namespace ionengine
{
    class UploadManager;

    class Mesh : public Entity
    {
      public:
        Mesh(rhi::RHI& RHI, UploadManager& uploadManager,
             std::unordered_map<uint64_t, core::ref_ptr<Surface>>& surfacesCache, asset::ModelFile const& modelFile);

        auto getSurfaces() const -> std::vector<core::ref_ptr<Surface>> const&;

        auto getMaterials() const -> std::vector<core::ref_ptr<Material>> const&;

        auto setMaterial(uint32_t const index, core::ref_ptr<Material> const& material) -> void;

      private:
        std::vector<core::ref_ptr<Surface>> surfaces;
        std::vector<core::ref_ptr<Material>> materials;
    };
} // namespace ionengine