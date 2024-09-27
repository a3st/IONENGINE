// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "fx.hpp"
#include "precompiled.h"

namespace ionengine::shadersys::fx
{
    auto sizeof_ShaderElementType(ShaderElementType const elementType) -> size_t
    {
        switch (elementType)
        {
            case ShaderElementType::Float4:
                return sizeof(float) * 4;
            case ShaderElementType::Float4x4:
                return sizeof(float) * 16;
            case ShaderElementType::Float3:
                return sizeof(float) * 3;
            case ShaderElementType::Float3x3:
                return sizeof(float) * 9;
            case ShaderElementType::Float2:
                return sizeof(float) * 2;
            case ShaderElementType::Float2x2:
                return sizeof(float) * 4;
            case ShaderElementType::Float:
                return sizeof(float);
            case ShaderElementType::Bool:
            case ShaderElementType::Uint:
                return sizeof(uint32_t);
            default:
                return 0;
        }
    }
} // namespace ionengine::shadersys::fx