// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

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

    std::shared_mutex mutable mutex;

    std::filesystem::path asset_path;

    Type const& get() const {
        return std::get<AssetStateData<Type, AssetStateType::Ok>>(data).asset;
    }

    Type& get() {
        return std::get<AssetStateData<Type, AssetStateType::Ok>>(data).asset;
    }

    bool is_ok() const {
        std::shared_lock lock(mutex);
        return data.index() == 0;
    }

    bool is_error() const {
        std::shared_lock lock(mutex);
        return data.index() == 1;
    }

    bool is_pending() const {
        std::shared_lock lock(mutex);
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

    void wait() {
        while(is_pending()) {
            std::this_thread::yield();
        }
    }
};

template<class Type>
using AssetPtr = std::shared_ptr<AssetState<Type>>;

template<class Type>
inline AssetPtr<Type> make_asset_ptr(std::filesystem::path const& asset_path) {
    return AssetPtr<Type>(new AssetState<Type> { .data = AssetStateData<Type, AssetStateType::Pending> { }, .asset_path = asset_path });
}

template<class Type>
inline AssetPtr<Type> make_asset_ptr(Type&& asset) {
    return AssetPtr<Type>(new AssetState<Type> { .data = AssetStateData<Type, AssetStateType::Ok> { .asset = std::move(asset) }, .asset_path = "" });
}

}
