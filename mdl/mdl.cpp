// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "mdl.hpp"
#include "precompiled.h"

namespace ionengine::asset::mdl
{
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
                return 0;
        }
    }
} // namespace ionengine::mdl