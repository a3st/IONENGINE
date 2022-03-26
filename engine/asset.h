// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/data.h>
#include <lib/thread_pool.h>

namespace ionengine::core {

struct MaterialData {

};

struct ShaderData {

};

struct TextureData {

};
    
using AssetData = std::variant<MaterialData, ShaderData, TextureData>;

class Asset {
public:

    Asset() = default;

private:

    AssetData _data;
};

class AssetManager {
public:

    AssetManager(ThreadPool& thread_pool);

    AssetManager(AssetManager const&) = delete;

    AssetManager(AssetManager&&) noexcept = delete;

    Handle<Job> load_asset_from_file(std::u8string_view const file_path, Handle<Asset>& dest);

private:

    ThreadPool* _thread_pool;
};

}
