// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <hlsl_spirv_package/hlsl_spirv.h>

namespace ionengine {

enum class HLSVFlags : uint32_t {
    HLSL = 1 << 0,
    SPIRV = 1 << 1
};

DECLARE_ENUM_CLASS_BIT_FLAG(HLSVFlags)

struct HLSVShader {
    std::u8string name;
    HLSVFlags flags;
    std::vector<char8_t> data;
};

class HLSVLoader {
public:

    using ConstIterator = std::vector<HLSVShader>::const_iterator;

    HLSVLoader() = default;

    bool parse(std::span<char8_t const> const data);

    ConstIterator begin() const { return _shaders.begin(); }
    ConstIterator end() const { return _shaders.end(); }

private:

    std::vector<HLSVShader> _shaders;
};

}
