// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/proxy_mutex.h>

namespace ionengine::asset {

enum class AssetStateType {
    Pending,
    Error,
    Ok
};

template<class AssetType, AssetStateType StateType>
struct AssetStateData { };

template<class AssetType>
struct AssetStateData<AssetType, AssetStateType::Ok> {
    AssetType asset;
};

template<class AssetType>
struct AssetStateData<AssetType, AssetStateType::Error> { };

template<class AssetType>
struct AssetStateData<AssetType, AssetStateType::Pending> { };

template<class Type>
struct AssetState {
    std::variant<
        AssetStateData<Type, AssetStateType::Ok>,
        AssetStateData<Type, AssetStateType::Error>,
        AssetStateData<Type, AssetStateType::Pending>
    > data;

    std::mutex mutex;

    std::filesystem::path asset_path;

    Type const& as_const_ok() const {
        return std::get<AssetStateData<Type, AssetStateType::Ok>>(data).asset;
    }

    lib::ProxyMutex<Type> as_ok() {
        return lib::ProxyMutex<Type>(&std::get<AssetStateData<Type, AssetStateType::Ok>>(data).asset, mutex);
    }

    bool is_ok() const {
        return data.index() == 0;
    }

    bool is_error() const {
        return data.index() == 1;
    }

    bool is_pending() const {
        return data.index() == 2;
    }

    std::filesystem::path const& path() const { 
        return asset_path;
    }

    void commit_ok(Type&& element) {
        std::lock_guard lock(mutex);
        data = AssetStateData<Type, AssetStateType::Ok> { .asset = std::move(element) };
    }

    void commit_error() {
        std::lock_guard lock(mutex);
        data = AssetStateData<Type, AssetStateType::Error> { };
    }
};

template<class Type>
using AssetPtr = std::shared_ptr<AssetState<Type>>;

template<class Type>
inline AssetPtr<Type> make_asset_ptr(std::filesystem::path const& asset_path) {
    return AssetPtr<Type>(new AssetState<Type> { .data = AssetStateData<Type, AssetStateType::Pending> { }, .asset_path = asset_path });
}

}
