// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/data.h>

namespace ionengine {

struct MeshSurfaceFile {
    uint32_t magic;
    renderer::MeshSurfaceData data;
};

struct TextureFile {
    uint32_t magic;
    // TODO!
};

struct ShaderPackageFile {
    uint32_t magic;
    renderer::ShaderPackageData data;
};

using AssetFile = std::variant<MeshSurfaceFile, TextureFile, ShaderPackageFile>;

}

namespace ionengine::tools {

class AssetCompiler {
public:

    AssetCompiler() = default;

    bool compile(std::filesystem::path const& file_path, AssetFile& asset_file);

    size_t serialize(std::span<char8_t>* const data, AssetFile const& asset_file);

    bool deserialize(std::span<char8_t const> const data, AssetFile& asset_file);

private:

    uint32_t const SHADER_MAGIC = ((uint32_t)(uint8_t)'S') | ((uint32_t)(uint8_t)'H' << 8) | ((uint32_t)(uint8_t)'A' << 16) | ((uint32_t)(uint8_t)'D' << 24);\
    uint32_t const MESH_SURFACE_MAGIC = ((uint32_t)(uint8_t)'M') | ((uint32_t)(uint8_t)'E' << 8) | ((uint32_t)(uint8_t)'S' << 16) | ((uint32_t)(uint8_t)'H' << 24);

    bool compile_shader_package(std::filesystem::path const& file_path, ShaderPackageFile& shader_package_file);
    
    size_t serialize_shader_package(std::span<char8_t>* const data, ShaderPackageFile const& shader_package_file);
    
    bool deserialize_shader_package(std::span<char8_t const> const data, ShaderPackageFile& shader_package_file);

    bool compile_mesh_surface(std::filesystem::path const& file_path, MeshSurfaceFile& mesh_surface_file);
    
    size_t serialize_mesh_surface(std::span<char8_t>* const data, MeshSurfaceFile const& mesh_surface_file);
    
    bool deserialize_mesh_surface(std::span<char8_t const> const data, MeshSurfaceFile& mesh_surface_file);
};

}
