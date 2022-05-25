// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_loader.h>
#include <asset/material.h>

namespace ionengine::asset {

class AssetManager;

template<>
class AssetLoader<Material> {
public:

    AssetLoader(AssetManager& asset_manager) :
        _asset_manager(&asset_manager) {

    }

    void load_asset(lib::Logger& logger, AssetPtr<Material> asset, lib::EventDispatcher<AssetEvent<Material>>& event_dispatcher) {

        std::filesystem::path path = asset.path();

        auto result = asset::Material::load_from_file(path, *_asset_manager);

        if(result.is_ok()) {
            asset.commit_ok(result.value(), path);
            event_dispatcher.broadcast(asset::AssetEvent<Material>::loaded(asset));
        } else {
            asset.commit_error(path);
            logger.warning(lib::LoggerCategoryType::Engine, std::format("the asset was not loaded, its path {}", path.string()));
        }
    }

private:

    asset::AssetManager* _asset_manager;
};

}
