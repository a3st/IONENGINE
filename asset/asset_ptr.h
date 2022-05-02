// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <asset/asset.h>
#include <lib/exception.h>

namespace ionengine::asset {

template<class Type = Asset>
class AssetPtr {
public:

    AssetPtr() = default;

    AssetPtr(std::filesystem::path const& path) {

        _ptr = std::make_shared<Type>(path);
        _ptr->commit(AssetState::Pending);
    }

    AssetPtr(AssetPtr const& other) {

        _ptr = other._ptr;
    }

    AssetPtr(AssetPtr&& other) noexcept {

        _ptr = std::move(other._ptr);
    }

    AssetPtr& operator=(AssetPtr const& other) {

        _ptr = other._ptr;
        return *this;
    }

    AssetPtr& operator=(AssetPtr&& other) {

        _ptr = std::move(other._ptr);
        return *this;
    }

    uint32_t use_count() const { return _ptr.use_count(); }

    Type* operator->() const {

        if(_ptr->state() == AssetState::Pending || _ptr->state() == AssetState::Error) {
            throw lib::Exception(std::format("Error while accessing an unloaded asset '{}'", _ptr->path().string()));
        }
        return _ptr.operator->(); 
    }

    Type& operator*() const { 

        if(_ptr->state() == AssetState::Pending || _ptr->state() == AssetState::Error) {
            throw lib::Exception(std::format("Error while accessing an unloaded asset '{}'", _ptr->path().string()));
        }
        return _ptr.operator*(); 
    }

    std::filesystem::path const& path() const { return _ptr->path(); }

    void commit(AssetState const state) { _ptr->commit(state); }

    AssetState state() const { return _ptr->state(); }

    explicit operator bool() const noexcept { return _ptr.operator bool(); }

private:

    std::shared_ptr<Type> _ptr;
};

}
