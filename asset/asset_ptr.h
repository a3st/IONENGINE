// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

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
    std::filesystem::path path;
};

template<class AssetType>
struct AssetStateData<AssetType, AssetStateType::Error> {
    std::filesystem::path path;
};

template<class AssetType>
struct AssetStateData<AssetType, AssetStateType::Pending> {
    std::filesystem::path path;
};

template<class Type>
struct AssetState {
    std::variant<
        AssetStateData<Type, AssetStateType::Ok>,
        AssetStateData<Type, AssetStateType::Error>,
        AssetStateData<Type, AssetStateType::Pending>
    > data;

    std::mutex mutex;
};

template<class Type>
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

        std::unique_lock lock(_ptr->mutex);
        if(_ptr->data.index() != 0) {
            throw lib::Exception("Error while accessing an unloaded asset");
        }
        return &(std::get<0>(_ptr->data).asset); 
    }

    Type& operator*() const { 

        std::unique_lock lock(_ptr->mutex);
        if(_ptr->data.index() != 0) {
            throw lib::Exception("Error while accessing an unloaded asset");
        }
        return std::get<0>(_ptr->data).asset;
    }

    std::filesystem::path const& path() const { 

        std::filesystem::path* path;
        auto state_visitor = make_visitor(
            [&](AssetStateData<Type, AssetStateType::Ok>& state) {
                path = &state.path;
            },
            [&](AssetStateData<Type, AssetStateType::Error>& state) {
                path = &state.path;
            },
            [&](AssetStateData<Type, AssetStateType::Pending>& state) {
                path = &state.path;
            }
        );

        std::unique_lock lock(_ptr->mutex);
        std::visit(state_visitor, _ptr->data);
        return *path; 
    }

    bool is_pending() const {
        std::unique_lock lock(_ptr->mutex);
        return _ptr->data.index() == 2;
    }

    bool is_ok() const {
        if(_ptr->mutex.try_lock()) {
            std::unique_lock lock(_ptr->mutex, std::adopt_lock);
            return _ptr->data.index() == 0;
        }
        return false;
    }

    bool is_error() const {
        if(_ptr->mutex.try_lock()) {
            std::unique_lock lock(_ptr->mutex, std::adopt_lock);
            return _ptr->data.index() == 1;
        }
        return false;
    }

    void wait() {
        while(is_pending()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    void commit_ok(Type&& element, std::filesystem::path const& asset_path) {
        std::unique_lock lock(_ptr->mutex);
        _ptr->data = AssetStateData<Type, AssetStateType::Ok> { .asset = std::move(element), .path = asset_path };
    }

    void commit_error(std::filesystem::path const& asset_path) {
        std::unique_lock lock(_ptr->mutex);
        _ptr->data = AssetStateData<Type, AssetStateType::Error> { .path = asset_path };
    }

    explicit operator bool() const noexcept { return _ptr.operator bool(); }

private:

    std::shared_ptr<AssetState<Type>> _ptr;
};

}
