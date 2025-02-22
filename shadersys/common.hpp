// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shadersys/hlslgen.hpp"

namespace ionengine::shadersys
{
    namespace common
    {
#pragma pack(push, 1)
        struct TRANSFORM_DATA
        {
            math::Mat4f modelViewProj;
        };

        struct SAMPLER_DATA
        {
            uint32_t linearSampler;

            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<sampler_t>("linearSampler");
            }
        };

        struct LIGHTING_DATA
        {
            uint32_t reserved;
        };

        struct MATERIAL_DATA
        {
        };
#pragma pack(pop)
    } // namespace common

    namespace constants
    {
        struct ScreenShaderData
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<cbuffer_t<common::SAMPLER_DATA>>("samplerBuffer");
                generator.property<cbuffer_t<common::MATERIAL_DATA>>("materialBuffer");
            }
        };
    } // namespace constants

    namespace inputs
    {
        struct StaticVSInput
        {
        };

        struct BaseVSInput
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<uint32_t>("id", "SV_VertexID");
            }
        };

        struct BaseVSOutput
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<math::Vec4f>("position", "SV_Position");
                generator.property<math::Vec2f>("uv", "TEXCOORD");
            }
        };

        struct BasePSOutput
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<math::Vec4f>("color", "SV_Target");
            }
        };
    } // namespace inputs
} // namespace ionengine::shadersys