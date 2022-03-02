// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/asset.h>

namespace ionengine {

class AssetCompiler {
public:

    AssetCompiler() = default;

    bool compile(std::filesystem::path const& file);

    const Asset* data() const { return &_asset; }

private:

    Asset _asset;
};

}