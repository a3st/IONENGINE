// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/asset.h>

namespace ionengine {

class AssetCompiler {
public:

    AssetCompiler() = default;

    bool compile(std::filesystem::path const& file);

    size_t serialize(std::vector<char8_t>& data);

    const std::variant<MeshFile, TextureFile>* data() const { return &_file; }

private:

    std::variant<MeshFile, TextureFile> _file;
};

}
