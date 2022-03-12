// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/asset_data.h>

#include <lib/handle_allocator.h>
#include <lib/thread_pool.h>

namespace ionengine {

using AssetDataId = uint32_t;

class AssetManager {
public:

    AssetManager(ThreadPool& thread_pool);

    AssetManager(AssetManager const&) = delete;

    AssetManager(AssetManager&&) noexcept = delete;

    template<class AssetType>
    Handle<Job> load_asset_data(AssetDataId const id, Handle<AssetData>& asset_data) {

        auto it = _asset_data_ids.find(id);
        
        if(it != _asset_data_ids.end()) {

            auto& _asset_data = _asset_data_allocator.get(it->second);
            
            if(!_asset_data.is_loaded()) {

                _asset_data.file.emplace<AssetType>();

                /*_thread_pool->push(
                    [&](AssetData& asset_data) {
                        
                        
                    },
                    std::ref(_asset_data)
                );*/
            }

        } else {

            auto _asset_data = AssetData {};
            _asset_data.file.emplace<AssetType>();
            _asset_data._file_path = _asset_paths[id];
            
            asset_data = _asset_data_allocator.allocate(std::move(_asset_data));
        }

        return INVALID_HANDLE(Job);
    }

    AssetData const& get_asset_data(Handle<AssetData> const& asset_data) const {

        return _asset_data_allocator.get(asset_data);
    }

private:

    ThreadPool* _thread_pool;

    HandleAllocator<AssetData> _asset_data_allocator;

    std::unordered_map<AssetDataId, Handle<AssetData>> _asset_data_ids;

    std::unordered_map<uint32_t, std::filesystem::path> _asset_paths;
};

}