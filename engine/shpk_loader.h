// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <hlsl_spirv_package/shpk.h>

namespace ionengine {

class ShpkLoader {
public:

    ShpkLoader() = default;

    bool parse(std::span<char8_t> const data);

    const HLSL_SPIRV_File* data() const { return &_hlsl_spirv_file; }

private:

    enum class Shpk : uint32_t {
        Magic = ((uint32_t)(uint8_t)'S') | ((uint32_t)(uint8_t)'H' << 8) | ((uint32_t)(uint8_t)'P' << 16) | ((uint32_t)(uint8_t)'K' << 24)
    };

    HLSL_SPIRV_File _hlsl_spirv_file;
};

}