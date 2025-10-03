// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shadersys/fx.hpp"
#include "shadersys/hlslgen.hpp"

namespace ionengine::shadersys
{
    namespace common
    {
        struct TransformData
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<core::Mat4f>("modelViewProj");
            }
        };

        struct SamplerData
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<uint32_t>("linearSampler");
            }
        };

        struct LightingData
        {
            uint32_t reserved;
        };

        struct EffectData
        {
        };

        struct PassData
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<uint32_t>("inputTexture1");
                generator.property<uint32_t>("inputTexture2");
                generator.property<uint32_t>("inputTexture3");
                generator.property<uint32_t>("inputTexture4");
                generator.property<uint32_t>("inputTexture5");
                generator.property<uint32_t>("inputTexture6");
                generator.property<uint32_t>("inputTexture7");
                generator.property<uint32_t>("inputTexture8");
            }
        };
    } // namespace common

    namespace constants
    {
        struct PostProcessShaderData
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<uint32_t>("gSamplerData", "packoffset(c0)");
                generator.property<uint32_t>("gEffectData", "packoffset(c1)");
                generator.property<uint32_t>("gPassData", "packoffset(c2)");
            }
        };

        struct SurfaceShaderData
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<uint32_t>("gTransformData", "packoffset(c0)");
                generator.property<uint32_t>("gSamplerData", "packoffset(c1)");
                generator.property<uint32_t>("gEffectData", "packoffset(c2)");
            }
        };

        struct UIShaderData
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<uint32_t>("gTransformData", "packoffset(c0)");
                generator.property<uint32_t>("gSamplerData", "packoffset(c1)");
                generator.property<uint32_t>("gEffectData", "packoffset(c2)");
            }
        };
    } // namespace constants

    namespace inputs
    {
        struct SurfaceVSInput
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<core::Vec3f>("position", "POSITION");
                generator.property<core::Vec3f>("normal", "NORMAL");
                generator.property<core::Vec2f>("uv", "TEXCOORD");

                if (generator.defined("USE_SKINNING"))
                {
                    generator.property<core::Vec4u>("boneIndices", "BLENDINDICES");
                    generator.property<core::Vec4f>("boneWeights", "BLENDWEIGHT");
                }
            }
        };

        struct SurfaceVSOutput
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<core::Vec4f>("position", "SV_Position");
                generator.property<core::Vec3f>("normal", "NORMAL");
                generator.property<core::Vec2f>("uv", "TEXCOORD");
            }
        };

        struct BaseVSInput
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<uint32_t>("id", "SV_VertexID");
            }
        };

        struct BaseVSOutput
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<core::Vec4f>("position", "SV_Position");
                generator.property<core::Vec2f>("uv", "TEXCOORD");
            }
        };

        struct UIVSInput
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<core::Vec2f>("position", "POSITION");
                generator.property<core::Vec2f>("uv", "TEXCOORD");
                generator.property<core::Vec4f>("color", "COLOR");
            }
        };

        struct UIVSOutput
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<core::Vec4f>("position", "SV_Position");
                generator.property<core::Vec2f>("uv", "TEXCOORD");
                generator.property<core::Vec4f>("color", "COLOR");
            }
        };

        struct ForwardPSOutput
        {
            static auto toHLSL(HLSLCodeGenerator& generator) -> void
            {
                generator.property<core::Vec4f>("color", "SV_Target");
            }
        };
    } // namespace inputs
} // namespace ionengine::shadersys