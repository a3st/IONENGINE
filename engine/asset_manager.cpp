// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/asset_manager.h>

#include <lib/hash/crc32.h>

using namespace ionengine;

AssetManager::AssetManager(ThreadPool& thread_pool) {

    _asset_paths["shader_package.asset"_hash] = "shader_package.asset";
}

