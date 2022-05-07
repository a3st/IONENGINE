// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_loader.h>
#include <asset/mesh.h>

namespace ionengine::asset {

template<>
class AssetLoader<Mesh> {
public:

    AssetLoader() = default;

    void load_asset(AssetPtr<Mesh> asset, lib::EventDispatcher<AssetEvent<Mesh>>& event_dispatcher) {

        std::filesystem::path path = asset.path();

        auto result = asset::Mesh::load_from_file(path);

        if(result.is_ok()) {
            asset.commit_ok(result.value(), path);
        } else {
            asset.commit_error(path);
        }

        event_dispatcher.broadcast(asset::AssetEvent<Mesh>::loaded(asset));
    }
};

}
