// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/asset_manager.h>
#include <asset/mesh.h>
#include <asset/technique.h>

using namespace ionengine::asset;

AssetManager::AssetManager(lib::ThreadPool& thread_pool) :
    _thread_pool(&thread_pool) {

}

AssetPtr<Mesh> AssetManager::get_mesh(std::filesystem::path const& asset_path) {

    return _mesh_pool.get(*_thread_pool, asset_path);
}

AssetPtr<Technique> AssetManager::get_technique(std::filesystem::path const& asset_path) {

    return _technique_pool.get(*_thread_pool, asset_path);
}

void AssetManager::update(float const delta_time) {
    _mesh_pool.update(delta_time);
    _technique_pool.update(delta_time);
}

AssetPool<Mesh>& AssetManager::mesh_pool() {
    return _mesh_pool;
}

AssetPool<Technique>& AssetManager::technique_pool() {
    return _technique_pool;
}
