// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::asset {

enum class AssetState {
    Pending,
    Error,
    Valid
};

class Asset {
public:

    virtual ~Asset() = default;

    Asset(std::filesystem::path const& asset_path) : _path(asset_path) { }

    virtual std::filesystem::path const& path() const { return _path; }

    virtual AssetState state() const { return _state; }

    virtual void commit(AssetState const state) {

        _state = state;
    }

private:

    std::filesystem::path _path;
    AssetState _state;
};

}
