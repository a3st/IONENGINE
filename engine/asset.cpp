// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/asset.h>
#include <asset_compiler/asset_compiler.h>
#include <lib/hash/crc32.h>
#include <lib/algorithm.h>

using namespace ionengine;
using namespace ionengine::core;
using ionengine::Handle;

AssetManager::AssetManager(ThreadPool& thread_pool) : _thread_pool(&thread_pool) {

}

Handle<Job> AssetManager::load_asset_from_file(std::u8string_view const file_path, Handle<Asset>& dest) {

    return INVALID_HANDLE(Job);
}

/*
Handle<JobData> AssetManager::load_asset_from_file(AssetDataId const id, Handle<AssetData>& asset_data) {

    Handle<JobData> job_data;
    auto it = _asset_streams.find(id);
        
    if(it != _asset_streams.end()) {

        it->second.job_data = _thread_pool->push(
            [&, this](AssetData& data, AssetStream& stream) {

                tools::AssetCompiler asset_compiler;

                std::vector<char8_t> file_data;
                size_t const file_size = get_file_size(stream.file_path, std::ios::binary);
                file_data.resize(file_size);

                if(!load_bytes_from_file(stream.file_path, file_data, std::ios::binary)) {
                    return;
                }

                AssetFile asset_file;
                bool result = asset_compiler.deserialize(file_data, asset_file);

                auto asset_visitor = make_visitor(
                    [&](ShaderPackageFile const& file) {
                        data = file.data;
                    },
                    [&](MeshSurfaceFile const& file) {
                        data = file.data;
                    },
                    [&](TextureFile const& file) {
                        // TODO!
                    }
                );

                std::visit(asset_visitor, asset_file);

                stream.is_loaded.store(result);
            },
            std::ref(_asset_datas[id]),
            std::ref(_asset_streams[id])
        );

        asset_data = Handle<AssetData>(id);
        job_data = it->second.job_data;

    } else {
        std::cerr << "[Error] AssetManager: The streaming asset was not found in the asset manager" << std::endl;
    }

    return job_data;
}

AssetData const& AssetManager::get_asset_data(Handle<AssetData> const& asset_data) {

    auto& stream = _asset_streams[asset_data.id];

    if(!stream.is_loaded.load()) {
        _thread_pool->wait(stream.job_data);
    }

    return _asset_datas[asset_data.id];
}
*/