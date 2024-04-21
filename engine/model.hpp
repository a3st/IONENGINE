// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset.hpp"
#include "extensions/importer.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class Material;
    class LinkedDevice;

    struct Primitive
    {
        core::ref_ptr<rhi::Buffer> vertices;
        core::ref_ptr<rhi::Buffer> indices;
        uint32_t index_count;
    };

    struct Mesh : public core::ref_counted_object
    {
        mutable core::ref_ptr<Material> material;
        std::vector<Primitive> primitives;
    };

    class Model : public Asset
    {
      public:
        Model(LinkedDevice& device);

        auto load_from_memory(std::span<uint8_t const> const data, ModelImporter& importer) -> bool;

        auto get_meshes() const -> std::vector<core::ref_ptr<Mesh>> const&;

      private:
        LinkedDevice* device;
        std::vector<core::ref_ptr<Mesh>> meshes;
    };
} // namespace ionengine