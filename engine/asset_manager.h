// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/data.h>
#include <lib/thread_pool.h>

namespace ionengine {

using AssetData = std::variant<renderer::MeshSurfaceData, renderer::ShaderPackageData>;

using AssetDataId = uint32_t;

class AssetManager {
public:

    AssetManager(ThreadPool& thread_pool);

    AssetManager(AssetManager const&) = delete;

    AssetManager(AssetManager&&) noexcept = delete;

    Handle<JobData> load_asset_from_file(AssetDataId const id, Handle<AssetData>& asset_data);

    AssetData const& get_asset_data(Handle<AssetData> const& asset_data);

private:

    struct AssetStream {
        std::filesystem::path file_path;
        Handle<JobData> job_data;
        std::atomic<bool> is_loaded;
    };

    ThreadPool* _thread_pool;

    std::unordered_map<AssetDataId, AssetData> _asset_datas;
    std::unordered_map<AssetDataId, AssetStream> _asset_streams;
};

}
