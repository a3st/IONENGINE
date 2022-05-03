// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_loader.h>
#include <asset/technique.h>

namespace ionengine::asset {

template<>
class AssetLoader<Technique> {
public:

    AssetLoader() = default;

    void load_asset(AssetPtr<Technique>& asset, lib::EventDispatcher<AssetEvent<Technique>>& event_dispatcher) {

        std::filesystem::path path = asset.path();

        std::cout << "Zdarova bidlo technique" << std::endl;

        auto result = asset::Technique::load_from_file(path);

        if(result.is_ok()) {
            asset.commit_ok(result.value());
        } else {
            asset.commit_error(path);
        }
    }
};

}
