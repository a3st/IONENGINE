// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shadersys/fx.hpp"
#include "shadersys/hlslgen.hpp"

namespace ionengine::shadersys
{
    namespace common
    {
        struct TRANSFORM_DATA
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<core::Mat4f>("modelViewProj");
            }

            static inline asset::fx::StructureData structureData{
                .name = "TRANSFORM_DATA",
                .elements = {{.name = "modelViewProj", .type = asset::fx::ElementType::Float4x4}},
                .size = 64};
        };

        struct SAMPLER_DATA
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<uint32_t>("linearSampler");
            }

            static inline asset::fx::StructureData structureData{
                .name = "SAMPLER_DATA",
                .elements = {{.name = "linearSampler", .type = asset::fx::ElementType::Uint}},
                .size = 4};
        };

        struct LIGHTING_DATA
        {
            uint32_t reserved;
        };

        struct EFFECT_DATA
        {
        };

        struct PASS_DATA
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
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

            static inline asset::fx::StructureData structureData{
                .name = "PASS_DATA",
                .elements = {{.name = "inputTexture1", .type = asset::fx::ElementType::Uint},
                             {.name = "inputTexture2", .type = asset::fx::ElementType::Uint},
                             {.name = "inputTexture3", .type = asset::fx::ElementType::Uint},
                             {.name = "inputTexture4", .type = asset::fx::ElementType::Uint},
                             {.name = "inputTexture5", .type = asset::fx::ElementType::Uint},
                             {.name = "inputTexture6", .type = asset::fx::ElementType::Uint},
                             {.name = "inputTexture7", .type = asset::fx::ElementType::Uint},
                             {.name = "inputTexture8", .type = asset::fx::ElementType::Uint}},
                .size = 32};
        };
    } // namespace common

    namespace constants
    {
        struct ScreenShaderData
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<uint32_t>("gSamplerData", "packoffset(c0)");
                generator.property<uint32_t>("gEffectData", "packoffset(c1)");
                generator.property<uint32_t>("gPassData", "packoffset(c2)");
            }

            static inline asset::fx::StructureData structureData{
                .name = "SHADER_DATA",
                .elements = {{.name = "gSamplerData", .type = asset::fx::ElementType::Uint},
                             {.name = "gEffectData", .type = asset::fx::ElementType::Uint},
                             {.name = "gPassData", .type = asset::fx::ElementType::Uint}},
                .size = 12};
        };

        struct SurfaceShaderData
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<uint32_t>("gTransformData", "packoffset(c0)");
                generator.property<uint32_t>("gSamplerData", "packoffset(c1)");
                generator.property<uint32_t>("gEffectData", "packoffset(c2)");
            }

            static inline asset::fx::StructureData structureData{
                .name = "SHADER_DATA",
                .elements = {{.name = "gTransformData", .type = asset::fx::ElementType::Uint},
                             {.name = "gSamplerData", .type = asset::fx::ElementType::Uint},
                             {.name = "gEffectData", .type = asset::fx::ElementType::Uint}},
                .size = 12};
        };
    } // namespace constants

    namespace inputs
    {
        struct StaticVSInput
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<core::Vec3f>("position", "POSITION");
                generator.property<core::Vec3f>("normal", "NORMAL");
                generator.property<core::Vec2f>("uv", "TEXCOORD");
            }

            static inline asset::fx::VertexLayoutData vertexLayout{
                .elements = {{.format = asset::fx::VertexFormat::RGB32_FLOAT, .semantic = "POSITION"},
                             {.format = asset::fx::VertexFormat::RGB32_FLOAT, .semantic = "NORMAL"},
                             {.format = asset::fx::VertexFormat::RG32_FLOAT, .semantic = "TEXCOORD"}},
                .size = 32};
        };

        struct StaticVSOutput
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<core::Vec4f>("position", "SV_Position");
                generator.property<core::Vec3f>("normal", "NORMAL");
                generator.property<core::Vec2f>("uv", "TEXCOORD");
            }
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
                generator.property<core::Vec4f>("position", "SV_Position");
                generator.property<core::Vec2f>("uv", "TEXCOORD");
            }
        };

        struct BasePSOutput
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<core::Vec4f>("color", "SV_Target");
            }
        };
    } // namespace inputs
} // namespace ionengine::shadersys