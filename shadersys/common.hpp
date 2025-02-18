// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/matrix.hpp"

namespace ionengine::shadersys::common
{
#pragma pack(push, 1)
    struct TransformData
    {
        math::Matf modelViewProj;
    };

    struct SamplerData
    {
        uint32_t linearSampler;
    };

    struct LightingData
    {
        uint32_t reserved;
    };
#pragma pack(pop)
} // namespace ionengine::shadersys::common