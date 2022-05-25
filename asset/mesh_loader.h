// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_loader.h>
#include <asset/mesh.h>

namespace ionengine::asset {

template<>
class AssetLoader<Mesh> {
public:

    AssetLoader() = default;

    void load_asset(lib::Logger& logger, AssetPtr<Mesh> asset, lib::EventDispatcher<AssetEvent<Mesh>>& event_dispatcher) {

        std::filesystem::path path = asset.path();

        auto result = asset::Mesh::load_from_file(path);

        if(result.is_ok()) {
            asset.commit_ok(result.value(), path);
            event_dispatcher.broadcast(asset::AssetEvent<Mesh>::loaded(asset));
        } else {
            asset.commit_error(path);
            logger.warning(lib::LoggerCategoryType::Engine, std::format("the asset was not loaded, its path {}", path.string()));
        }
    }
};

}
