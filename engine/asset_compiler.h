// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/asset.h>

namespace ionengine {

struct ShaderFile {
    uint32_t magic;
    uint32_t count;
    std::unique_ptr<renderer::ShaderData[]> data;
};

using AssetFile = std::variant<MeshFile, TextureFile, ShaderFile>;

class AssetCompiler {
public:

    AssetCompiler() = default;

    bool compile(std::filesystem::path const& file_path, AssetFile& asset_file);

    size_t serialize(std::vector<char8_t>& data);
    bool deserialize(std::vector<char8_t> const& data);

private:

    enum class AssetFile : uint32_t {
        MeshMagic = ((uint32_t)(uint8_t)'M') | ((uint32_t)(uint8_t)'E' << 8) | ((uint32_t)(uint8_t)'S' << 16) | ((uint32_t)(uint8_t)'H' << 24),
        TextureMagic = ((uint32_t)(uint8_t)'T') | ((uint32_t)(uint8_t)'E' << 8) | ((uint32_t)(uint8_t)'X' << 16) | ((uint32_t)(uint8_t)'T' << 24),
        ShaderMagic = ((uint32_t)(uint8_t)'S') | ((uint32_t)(uint8_t)'H' << 8) | ((uint32_t)(uint8_t)'A' << 16) | ((uint32_t)(uint8_t)'D' << 24)
    };

    bool compile_shader(std::filesystem::path const& file);
};

}
