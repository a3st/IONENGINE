// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "asset_loader.hpp"
#include "extensions/importers/glb.hpp"
#include "linked_device.hpp"
#include "precompiled.h"

namespace ionengine
{
    AssetLoader::AssetLoader(LinkedDevice& device) : device(&device)
    {
    }

    auto AssetLoader::load_model(std::filesystem::path const& file_path) -> core::ref_ptr<Model>
    {
        auto model = core::make_ref<Model>(*device);

        std::vector<uint8_t> data;
        {
            std::basic_ifstream<uint8_t> ifs(file_path, std::ios::binary);
            ifs.seekg(0, std::ios::end);
            size_t const size = ifs.tellg();
            ifs.seekg(0, std::ios::beg);
            data.resize(size);
            ifs.read(data.data(), data.size());
        }

        std::unique_ptr<ModelImporter> importer;

        if (file_path.extension().compare(".glb") == 0)
        {
            importer = std::make_unique<GLBImporter>();
        }

        model->load_from_memory(data, *importer);
        return model;
    }

    auto AssetLoader::load_texture(std::filesystem::path const& file_path) -> core::ref_ptr<Texture>
    {
        return nullptr;
    }

    auto AssetLoader::load_material(std::filesystem::path const& file_path) -> core::ref_ptr<Material>
    {
        return nullptr;
    }
} // namespace ionengine