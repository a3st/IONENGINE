// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/asset_manager.h>
#include <lib/scope_profiler.h>

using namespace ionengine;
using namespace ionengine::asset;

AssetManager::AssetManager() :
    _mesh_pool(std::make_unique<AssetLoader<Mesh>>()),
    _material_pool(std::make_unique<AssetLoader<Material>>(*this)),
    _texture_pool(std::make_unique<AssetLoader<Texture>>()),
    _shader_pool(std::make_unique<AssetLoader<Shader>>()) {

}

AssetPtr<Mesh> AssetManager::get_mesh(std::filesystem::path const& asset_path) {
    return _mesh_pool.get(asset_path);
}

AssetPtr<Material> AssetManager::get_material(std::filesystem::path const& asset_path) {
    return _material_pool.get(asset_path);
}

AssetPtr<Texture> AssetManager::get_texture(std::filesystem::path const& asset_path) {
    return _texture_pool.get(asset_path);
}

AssetPtr<Shader> AssetManager::get_shader(std::filesystem::path const& asset_path) {
    return _shader_pool.get(asset_path);
}

void AssetManager::update(float const delta_time) {
    SCOPE_PROFILE()

    _mesh_pool.update(delta_time);
    _material_pool.update(delta_time);
    _texture_pool.update(delta_time);
    _shader_pool.update(delta_time);
}

AssetPool<Mesh>& AssetManager::mesh_pool() {
    return _mesh_pool;
}

AssetPool<Material>& AssetManager::material_pool() {
    return _material_pool;
}

AssetPool<Texture>& AssetManager::texture_pool() {
    return _texture_pool;
}

AssetPool<Shader>& AssetManager::shader_pool() {
    return _shader_pool;
}
