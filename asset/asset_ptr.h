// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>
#include <lib/exception.h>

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
struct AssetStateData<AssetType, AssetStateType::Error> {
    std::filesystem::path path;
};

template<class AssetType>
struct AssetStateData<AssetType, AssetStateType::Pending> {
    std::filesystem::path path;
};

template<class Type = Asset>
struct AssetState {
    std::variant<
        AssetStateData<Type, AssetStateType::Ok>,
        AssetStateData<Type, AssetStateType::Error>,
        AssetStateData<Type, AssetStateType::Pending>
    > data;
};

template<class Type = Asset>
class AssetPtr {
public:

    AssetPtr() = default;

    constexpr AssetPtr(std::nullptr_t) noexcept { }

    AssetPtr(std::filesystem::path const& asset_path) {
        _ptr = std::make_shared<AssetState<Type>>(
            AssetStateData<Type, AssetStateType::Pending> { .path = asset_path }
        );
    }

    AssetPtr(AssetPtr const& other) = default;

    AssetPtr(AssetPtr&&) noexcept = default;

    AssetPtr& operator=(AssetPtr const&) = default;

    AssetPtr& operator=(AssetPtr&&) noexcept = default;

    uint32_t use_count() const { return _ptr.use_count(); }

    Type* operator->() const {

        if(_ptr->data.index() != 0) {
            throw lib::Exception(std::format("Error while accessing an unloaded asset '{}'", path().string()));
        }
        return &(std::get<0>(_ptr->data).asset); 
    }

    Type& operator*() const { 

        if(_ptr->data.index() != 0) {
            throw lib::Exception(std::format("Error while accessing an unloaded asset '{}'", path().string()));
        }
        return std::get<0>(_ptr->data).asset;
    }

    std::filesystem::path const& path() const { 

        std::filesystem::path* path;
        auto state_visitor = make_visitor(
            [&](AssetStateData<Type, AssetStateType::Ok>& state) {

            },
            [&](AssetStateData<Type, AssetStateType::Error>& state) {
                path = &state.path;
            },
            [&](AssetStateData<Type, AssetStateType::Pending>& state) {
                path = &state.path;
            }
        );

        std::visit(state_visitor, _ptr->data);
        return *path; 
    }

    bool is_pending() const {
        return _ptr->data.index() == 2;
    }

    bool is_ok() const {
        return _ptr->data.index() == 0;
    }

    bool is_error() const {
        return _ptr->data.index() == 1;
    }

    void commit_ok(Type&& element) {
        _ptr->data = AssetStateData<Type, AssetStateType::Ok> { .asset = std::move(element) };
    }

    void commit_error(std::filesystem::path const& asset_path) {
        _ptr->data = AssetStateData<Type, AssetStateType::Error> { .path = asset_path };
    }

    explicit operator bool() const noexcept { return _ptr.operator bool(); }

private:

    std::shared_ptr<AssetState<Type>> _ptr;
};

}
