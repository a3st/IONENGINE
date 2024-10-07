// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine::shadersys
{
    namespace fx
    {
        std::array<uint8_t, 4> constexpr Magic{'F', 'X', '1', '0'};

        enum class APIType : uint32_t
        {
            DXIL,
            SPIRV
        };

        enum class VertexFormat
        {
            R32_UINT,
            R32_SINT,
            R32_FLOAT,
            RG32_UINT,
            RG32_SINT,
            RG32_FLOAT,
            RGB32_UINT,
            RGB32_SINT,
            RGB32_FLOAT,
            RGBA32_UINT,
            RGBA32_SINT,
            RGBA32_FLOAT
        };

        auto sizeof_VertexFormat(VertexFormat const format) -> size_t;

        enum class ElementType
        {
            Float4x4,
            Float3x3,
            Float2x2,
            Float4,
            Float3,
            Float2,
            Float,
            Uint,
            Sint,
            Bool
        };

        auto sizeof_ElementType(ElementType const elementType) -> size_t;

        enum class StageType
        {
            Vertex,
            Pixel,
            Compute
        };

        enum class CullSide
        {
            Back,
            Front,
            None
        };

        struct InputElementData
        {
            VertexFormat format;
            std::string semantic;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(format, "format");
                archive.property(semantic, "semantic");
            }
        };

        struct ConstantData
        {
            std::string name;
            ElementType type;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(type, "type");
            }
        };

        struct StructureElementData
        {
            std::string name;
            ElementType type;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(type, "type");
            }
        };

        struct InputData
        {
            std::vector<InputElementData> elements;
            uint32_t size;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(elements, "elements");
                archive.property(size, "sizeInBytes");
            }
        };

        struct StructureData
        {
            std::string name;
            std::vector<StructureElementData> elements;
            uint32_t size;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(elements, "elements");
                archive.property(size, "sizeInBytes");
            }
        };

        struct StageData
        {
            uint32_t buffer;
            std::string entryPoint;
            InputData input;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(buffer, "buffer");
                archive.property(entryPoint, "entryPoint");
                archive.property(input, "input");
            }
        };

        struct HeaderData
        {
            std::string name;
            std::string description;
            std::string domain;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(description, "description");
                archive.property(domain, "domain");
            }
        };

        struct BufferData
        {
            uint64_t offset;
            size_t size;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(offset, "offset");
                archive.property(size, "sizeInBytes");
            }
        };

        struct OutputData
        {
            std::unordered_map<StageType, StageData> stages;
            bool depthWrite;
            bool stencilWrite;
            CullSide cullSide;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(stages, "stages");
                archive.property(depthWrite, "depthWrite");
                archive.property(stencilWrite, "stencilWrite");
                archive.property(cullSide, "cullSide");
            }
        };

        struct EffectData
        {
            HeaderData header;
            OutputData output;
            std::vector<ConstantData> constants;
            std::vector<StructureData> structures;
            std::vector<BufferData> buffers;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(header, "header");
                archive.property(output, "output");
                archive.property(constants, "constants");
                archive.property(structures, "structures");
                archive.property(buffers, "buffers");
            }
        };
    } // namespace fx

    struct ShaderEffectFile
    {
        std::array<uint8_t, fx::Magic.size()> magic;
        fx::APIType apiType;
        fx::EffectData effectData;
        std::vector<uint8_t> blob;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.property(apiType);
            archive.template with<core::serialize_ojson, core::serialize_ijson>(effectData);
            archive.property(blob);
        }
    };
} // namespace ionengine::shadersys

namespace ionengine::core
{
    template <>
    struct serializable_enum<shadersys::fx::VertexFormat>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(shadersys::fx::VertexFormat::RGBA32_FLOAT, "RGBA32_FLOAT");
            archive.field(shadersys::fx::VertexFormat::RGBA32_SINT, "RGBA32_SINT");
            archive.field(shadersys::fx::VertexFormat::RGBA32_UINT, "RGBA32_UINT");
            archive.field(shadersys::fx::VertexFormat::RGB32_FLOAT, "RGB32_FLOAT");
            archive.field(shadersys::fx::VertexFormat::RGB32_SINT, "RGB32_SINT");
            archive.field(shadersys::fx::VertexFormat::RGB32_UINT, "RGB32_UINT");
            archive.field(shadersys::fx::VertexFormat::RG32_FLOAT, "RG32_FLOAT");
            archive.field(shadersys::fx::VertexFormat::RG32_SINT, "RG32_SINT");
            archive.field(shadersys::fx::VertexFormat::RG32_UINT, "RG32_UINT");
            archive.field(shadersys::fx::VertexFormat::R32_FLOAT, "R32_FLOAT");
            archive.field(shadersys::fx::VertexFormat::R32_SINT, "R32_SINT");
            archive.field(shadersys::fx::VertexFormat::R32_UINT, "R32_UINT");
        }
    };

    template <>
    struct serializable_enum<shadersys::fx::ElementType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(shadersys::fx::ElementType::Float4x4, "FLOAT4x4");
            archive.field(shadersys::fx::ElementType::Float3x3, "FLOAT3x3");
            archive.field(shadersys::fx::ElementType::Float2x2, "FLOAT2x2");
            archive.field(shadersys::fx::ElementType::Float4, "FLOAT4");
            archive.field(shadersys::fx::ElementType::Float3, "FLOAT3");
            archive.field(shadersys::fx::ElementType::Float2, "FLOAT2");
            archive.field(shadersys::fx::ElementType::Float, "FLOAT");
            archive.field(shadersys::fx::ElementType::Uint, "UINT");
            archive.field(shadersys::fx::ElementType::Sint, "SINT");
            archive.field(shadersys::fx::ElementType::Bool, "BOOL");
        }
    };

    template <>
    struct serializable_enum<shadersys::fx::StageType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(shadersys::fx::StageType::Vertex, "VS");
            archive.field(shadersys::fx::StageType::Pixel, "PS");
            archive.field(shadersys::fx::StageType::Compute, "CS");
        }
    };

    template <>
    struct serializable_enum<shadersys::fx::CullSide>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(shadersys::fx::CullSide::None, "NONE");
            archive.field(shadersys::fx::CullSide::Back, "BACK");
            archive.field(shadersys::fx::CullSide::Front, "FRONT");
        }
    };
} // namespace ionengine::core