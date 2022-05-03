// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_loader.h>
#include <asset/mesh.h>

namespace ionengine::asset {

template<>
class AssetLoader<Mesh> {
public:

    AssetLoader() = default;

    void load_asset(AssetPtr<Mesh>& asset, lib::EventDispatcher<AssetEvent<Mesh>>& event_dispatcher) {

        std::cout << "Zdarova bidlo mesh" << std::endl;
    }
};

}
