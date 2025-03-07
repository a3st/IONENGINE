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
                .size = 16};
        };

        struct SAMPLER_DATA
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<sampler_t>("linearSampler");
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
                generator.property<texture2D_t>("inputTexture1");
                generator.property<texture2D_t>("inputTexture2");
                generator.property<texture2D_t>("inputTexture3");
                generator.property<texture2D_t>("inputTexture4");
                generator.property<texture2D_t>("inputTexture5");
                generator.property<texture2D_t>("inputTexture6");
                generator.property<texture2D_t>("inputTexture7");
                generator.property<texture2D_t>("inputTexture8");
            }

            static inline asset::fx::StructureData structureData{
                .name = "PASS_DATA",
                .elements = {{.name = "inputTexture1", .type = asset::fx::ElementType::Texture2D},
                             {.name = "inputTexture2", .type = asset::fx::ElementType::Texture2D},
                             {.name = "inputTexture3", .type = asset::fx::ElementType::Texture2D},
                             {.name = "inputTexture4", .type = asset::fx::ElementType::Texture2D},
                             {.name = "inputTexture5", .type = asset::fx::ElementType::Texture2D},
                             {.name = "inputTexture6", .type = asset::fx::ElementType::Texture2D},
                             {.name = "inputTexture7", .type = asset::fx::ElementType::Texture2D},
                             {.name = "inputTexture8", .type = asset::fx::ElementType::Texture2D}},
                .size = 32};
        };
    } // namespace common

    namespace constants
    {
        struct ScreenShaderData
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<cbuffer_t<common::SAMPLER_DATA>>("samplerData");
                generator.property<cbuffer_t<common::EFFECT_DATA>>("effectData");
                generator.property<cbuffer_t<common::PASS_DATA>>("passData");
            }

            static inline asset::fx::StructureData structureData{
                .name = "SHADER_DATA",
                .elements = {{.name = "samplerData", .type = asset::fx::ElementType::ConstBuffer},
                             {.name = "effectData", .type = asset::fx::ElementType::ConstBuffer},
                             {.name = "passData", .type = asset::fx::ElementType::ConstBuffer}},
                .size = 8};
        };

        struct SurfaceShaderData
        {
            static auto toHLSL(HLSLCodeGen& generator) -> void
            {
                generator.property<cbuffer_t<common::TRANSFORM_DATA>>("transformData");
                generator.property<cbuffer_t<common::SAMPLER_DATA>>("samplerData");
                generator.property<cbuffer_t<common::EFFECT_DATA>>("effectData");
            }

            static inline asset::fx::StructureData structureData{
                .name = "SHADER_DATA",
                .elements = {{.name = "transformData", .type = asset::fx::ElementType::ConstBuffer},
                             {.name = "samplerData", .type = asset::fx::ElementType::ConstBuffer},
                             {.name = "effectData", .type = asset::fx::ElementType::ConstBuffer}},
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