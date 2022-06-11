// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_loader.h>
#include <asset/texture.h>

namespace ionengine::asset {

template<>
class AssetLoader<Texture> {
public:

    AssetLoader() = default;

    void load_asset(AssetPtr<Texture> asset, lib::EventDispatcher<AssetEvent<Texture>>& event_dispatcher) {

        std::filesystem::path const& path = asset->path();

        auto result = asset::Texture::load_from_file(path);

        if(result.is_ok()) {
            asset->commit_ok(result.as_ok());
            event_dispatcher.broadcast(asset::AssetEvent<Texture>::loaded(asset));
        } else {
            asset->commit_error();
            lib::logger().warning(lib::LoggerCategoryType::Engine, std::format("the asset was not loaded, its path {}", path.string()));
        }   
    }
};

}
