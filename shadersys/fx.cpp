// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "fx.hpp"
#include "precompiled.h"

namespace ionengine::asset::fx
{
    auto sizeof_ElementType(ElementType const elementType) -> size_t
    {
        switch (elementType)
        {
            case ElementType::Float4:
                return sizeof(float) * 4;
            case ElementType::Float4x4:
                return sizeof(float) * 16;
            case ElementType::Float3:
                return sizeof(float) * 3;
            case ElementType::Float3x3:
                return sizeof(float) * 9;
            case ElementType::Float2:
                return sizeof(float) * 2;
            case ElementType::Float2x2:
                return sizeof(float) * 4;
            case ElementType::Float:
                return sizeof(float);
            case ElementType::Bool:
            case ElementType::Uint:
                return sizeof(uint32_t);
            case ElementType::Sint:
                return sizeof(int32_t);
            default:
                throw std::invalid_argument("Unknown ElementType value");
        }
    }
} // namespace ionengine::asset::fx