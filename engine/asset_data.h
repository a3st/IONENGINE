// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/data.h>

namespace ionengine {

struct MeshFile {
    uint32_t magic;
    // TODO!
};

struct TextureFile {
    uint32_t magic;
    // TODO!
};

struct ShaderFile {
    uint32_t magic;
    uint32_t count;
    std::unique_ptr<renderer::ShaderData[]> data;
};

using AssetFile = std::variant<MeshFile, TextureFile, ShaderFile>;

struct AssetData {
    std::filesystem::path _file_path;
    AssetFile file;
    bool _is_loaded;

    std::filesystem::path const& file_path() const { return _file_path; }

    bool is_loaded() const { return _is_loaded; }
};

}
