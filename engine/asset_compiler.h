// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <engine/asset.h>

namespace ionengine {

class AssetCompiler {
public:

    AssetCompiler() = default;

    bool compile(std::filesystem::path const& file);

    size_t serialize(std::vector<char8_t>& data);
    bool deserialize(std::vector<char8_t> const& data);

    const std::variant<MeshFile, TextureFile>* data() const { return &_file; }

private:

    enum class Asset : uint32_t {
        MeshMagic = ((uint32_t)(uint8_t)'M') | ((uint32_t)(uint8_t)'E' << 8) | ((uint32_t)(uint8_t)'S' << 16) | ((uint32_t)(uint8_t)'H' << 24),
        TextureMagic = ((uint32_t)(uint8_t)'T') | ((uint32_t)(uint8_t)'E' << 8) | ((uint32_t)(uint8_t)'X' << 16) | ((uint32_t)(uint8_t)'0' << 24)
    };

    std::variant<MeshFile, TextureFile> _file;
};

}
