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

}

namespace ionengine::tools {

class AssetCompiler {
public:

    AssetCompiler() = default;

    bool compile(std::filesystem::path const& file_path, AssetFile& asset_file);

    size_t serialize(std::vector<char8_t>& data, AssetFile const& asset_file);

    bool deserialize(std::vector<char8_t> const& data, AssetFile& asset_file);

private:

    uint32_t const SHADER_MAGIC = ((uint32_t)(uint8_t)'S') | ((uint32_t)(uint8_t)'H' << 8) | ((uint32_t)(uint8_t)'A' << 16) | ((uint32_t)(uint8_t)'D' << 24);

    bool compile_shader(std::filesystem::path const& file_path, ShaderFile& shader_file);

    size_t serialize_shader(std::vector<char8_t>& data, ShaderFile const& shader_file);

    bool deserialize_shader(std::vector<char8_t> const& data, ShaderFile& shader_file);
};

}
