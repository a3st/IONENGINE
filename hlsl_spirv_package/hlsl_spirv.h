// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace hlsl_spirv_package {

enum class HLSL_SPIRV_Flags : uint32_t {
    HLSL = 1 << 0,
    SPIRV = 1 << 1
};

DECLARE_ENUM_CLASS_BIT_FLAG(HLSL_SPIRV_Flags)

const uint32_t HLSL_SPIRV_Magic = ((uint32_t)(uint8_t)'H') | ((uint32_t)(uint8_t)'L' << 8) | ((uint32_t)(uint8_t)'S' << 16) | ((uint32_t)(uint8_t)'V' << 24);

struct HLSL_SPIRV_Header {
    uint32_t flags;
    uint32_t count;
};

struct HLSL_SPIRV_Shader {
    char8_t name[48];
    uint32_t flags;
    size_t size;
};

struct HLSL_SPIRV_File {
    uint32_t magic;
    HLSL_SPIRV_Header header;
    std::unique_ptr<HLSL_SPIRV_Shader[]> shaders;
    std::unique_ptr<char8_t[]> data;
};

}
