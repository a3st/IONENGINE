// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_loader.h>
#include <asset/shader.h>

namespace ionengine::asset {

template<>
class AssetLoader<Shader> {
public:

    AssetLoader() = default;

    void load_asset(AssetPtr<Shader> asset, lib::EventDispatcher<AssetEvent<Shader>>& event_dispatcher) {

        std::filesystem::path const& path = asset->path();

        auto result = asset::Shader::load_from_file(path);

        if(result.is_ok()) {
            asset->commit_ok(result.as_ok());
            event_dispatcher.broadcast(asset::AssetEvent<Shader>::loaded(asset));
        } else {
            asset->commit_error();
            lib::logger().warning(lib::LoggerCategoryType::Engine, std::format("the asset was not loaded, its path {}", path.string()));
        }
    }
};

}
