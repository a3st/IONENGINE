// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <scene/subscene.h>
#include <asset/asset_manager.h>
#include <lib/exception.h>

namespace ionengine::scene {

template<class Type>
struct CacheEntry {
    Type value;
    uint64_t hash;
};

class SubsceneCache {
public:

    SubsceneCache() {
        _data.reserve(10);
    }

    Subscene& get(asset::AssetManager& asset_manager, std::filesystem::path const& subscene_path) {

        auto result = Subscene::load_from_file(subscene_path, asset_manager);
    
        if(result.is_ok()) {
            auto subscene = result.as_ok();
            return _data.emplace_back(std::move(subscene));
        } else {
            throw lib::Exception("123");
        }
    }

private:

    std::vector<Subscene> _data;
};

}