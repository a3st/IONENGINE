// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset_ptr.h>

namespace ionengine::asset {

enum class AssetEventType {
    Loaded,
    Unloaded,
    Added,
    Removed
};

template<class AssetType, AssetEventType EventType>
struct AssetEventData { };

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Loaded> {
    AssetPtr<AssetType> asset;
};

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Unloaded> {
    AssetPtr<AssetType> asset;
};

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Added> {
    AssetPtr<AssetType> asset;
};

template<class AssetType>
struct AssetEventData<AssetType, AssetEventType::Removed> {
    std::filesystem::path path;
};

template<class Type>
struct AssetEvent {
    std::variant<
        AssetEventData<Type, AssetEventType::Loaded>,
        AssetEventData<Type, AssetEventType::Unloaded>,
        AssetEventData<Type, AssetEventType::Added>,
        AssetEventData<Type, AssetEventType::Removed>
    > data;

    static AssetEvent<Type> removed(std::filesystem::path const& path) {
        return AssetEvent<Type> {
            .data = AssetEventData<Type, AssetEventType::Removed> { .path = path }
        };
    }

    static AssetEvent<Type> added(AssetPtr<Type> const& asset) {
        return AssetEvent<Type> {
            .data = AssetEventData<Type, AssetEventType::Added> { .asset = asset }
        };
    }

    static AssetEvent<Type> loaded(AssetPtr<Type> const& asset) {
        return AssetEvent<Type> {
            .data = AssetEventData<Type, AssetEventType::Loaded> { .asset = asset }
        };
    }
};

}
