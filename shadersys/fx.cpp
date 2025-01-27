// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

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
                throw std::invalid_argument("the argument cannot be converted into");
        }
    }

    auto sizeof_VertexFormat(VertexFormat const format) -> size_t
    {
        switch (format)
        {
            case VertexFormat::RGBA32_FLOAT:
                return sizeof(float) * 4;
            case VertexFormat::RGBA32_SINT:
                return sizeof(int32_t) * 4;
            case VertexFormat::RGBA32_UINT:
                return sizeof(uint32_t) * 4;
            case VertexFormat::RGB32_FLOAT:
                return sizeof(float) * 3;
            case VertexFormat::RGB32_SINT:
                return sizeof(int32_t) * 3;
            case VertexFormat::RGB32_UINT:
                return sizeof(uint32_t) * 3;
            case VertexFormat::RG32_FLOAT:
                return sizeof(float) * 2;
            case VertexFormat::RG32_SINT:
                return sizeof(int32_t) * 2;
            case VertexFormat::RG32_UINT:
                return sizeof(uint32_t) * 2;
            case VertexFormat::R32_FLOAT:
                return sizeof(float);
            case VertexFormat::R32_SINT:
                return sizeof(int32_t);
            case VertexFormat::R32_UINT:
                return sizeof(uint32_t);
            default:
                throw std::invalid_argument("the argument cannot be converted into");
        }
    }
} // namespace ionengine::asset::fx