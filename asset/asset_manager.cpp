// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/asset_manager.h>
#include <asset/mesh.h>
#include <asset/technique.h>

using namespace ionengine::asset;

AssetManager::AssetManager() {

    
}

AssetPtr<Mesh> AssetManager::get_mesh(std::filesystem::path const& file_path) {

    return _meshes.get(file_path);
}

AssetPtr<Technique> AssetManager::get_technique(std::filesystem::path const& file_path) {

    return _techniques.get(file_path);
}

void AssetManager::update(float const delta_time) {

    _meshes.update(delta_time);
}