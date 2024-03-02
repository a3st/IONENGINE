// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "material.hpp"
#include "model.hpp"
#include "texture.hpp"

namespace ionengine
{
    class LinkedDevice;

    class AssetLoader
    {
      public:
        AssetLoader(LinkedDevice& device);

        auto load_model(std::filesystem::path const& file_path) -> core::ref_ptr<Model>;

        auto load_texture(std::filesystem::path const& file_path) -> core::ref_ptr<Texture>;

        auto load_material(std::filesystem::path const& file_path) -> core::ref_ptr<Material>;

      private:
        LinkedDevice* device;
    };
} // namespace ionengine