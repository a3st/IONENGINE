// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine::shadersys
{
    namespace fx
    {
        std::array<uint8_t, 4> constexpr Magic{'F', 'X', '1', '0'};

        enum class ShaderAPIType : uint32_t
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

        enum class ShaderElementType
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

        auto sizeof_ShaderElementType(ShaderElementType const elementType) -> size_t;

        enum class ShaderStageType
        {
            Vertex,
            Pixel,
            Compute
        };

        enum class ShaderCullSide
        {
            Back,
            Front,
            None
        };

        struct ShaderInputElementData
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

        struct ShaderConstantData
        {
            std::string name;
            ShaderElementType type;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(type, "type");
            }
        };

        struct ShaderStructureElementData
        {
            std::string name;
            ShaderElementType type;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(type, "type");
            }
        };

        struct ShaderInputData
        {
            std::vector<ShaderInputElementData> elements;
            uint32_t size;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(elements, "elements");
                archive.property(size, "sizeInBytes");
            }
        };

        struct ShaderStructureData
        {
            std::string name;
            std::vector<ShaderStructureElementData> elements;
            uint32_t size;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(elements, "elements");
                archive.property(size, "sizeInBytes");
            }
        };

        struct ShaderStageData
        {
            uint32_t buffer;
            std::string entryPoint;
            ShaderInputData inputData;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(buffer, "buffer");
                archive.property(entryPoint, "entryPoint");
                archive.property(inputData, "input");
            }
        };

        struct ShaderHeaderData
        {
            std::string shaderName;
            std::string description;
            std::string shaderDomain;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(shaderName, "name");
                archive.property(description, "description");
                archive.property(shaderDomain, "domain");
            }
        };

        struct ShaderBufferData
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

        struct ShaderOutputData
        {
            std::unordered_map<ShaderStageType, ShaderStageData> stages;
            bool depthWrite;
            bool stencilWrite;
            ShaderCullSide cullSide;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(stages, "stages");
                archive.property(depthWrite, "depthWrite");
                archive.property(stencilWrite, "stencilWrite");
                archive.property(cullSide, "cullSide");
            }
        };

        struct ShaderEffectData
        {
            ShaderHeaderData header;
            ShaderOutputData output;
            std::vector<ShaderConstantData> constants;
            std::vector<ShaderStructureData> structures;
            std::vector<ShaderBufferData> buffers;

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
        fx::ShaderAPIType apiType;
        fx::ShaderEffectData effectData;
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
    struct serializable_enum<shadersys::fx::ShaderElementType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(shadersys::fx::ShaderElementType::Float4x4, "FLOAT4x4");
            archive.field(shadersys::fx::ShaderElementType::Float3x3, "FLOAT3x3");
            archive.field(shadersys::fx::ShaderElementType::Float2x2, "FLOAT2x2");
            archive.field(shadersys::fx::ShaderElementType::Float4, "FLOAT4");
            archive.field(shadersys::fx::ShaderElementType::Float3, "FLOAT3");
            archive.field(shadersys::fx::ShaderElementType::Float2, "FLOAT2");
            archive.field(shadersys::fx::ShaderElementType::Float, "FLOAT");
            archive.field(shadersys::fx::ShaderElementType::Uint, "UINT");
            archive.field(shadersys::fx::ShaderElementType::Sint, "SINT");
            archive.field(shadersys::fx::ShaderElementType::Bool, "BOOL");
        }
    };

    template <>
    struct serializable_enum<shadersys::fx::ShaderStageType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(shadersys::fx::ShaderStageType::Vertex, "VS");
            archive.field(shadersys::fx::ShaderStageType::Pixel, "PS");
            archive.field(shadersys::fx::ShaderStageType::Compute, "CS");
        }
    };

    template <>
    struct serializable_enum<shadersys::fx::ShaderCullSide>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(shadersys::fx::ShaderCullSide::None, "NONE");
            archive.field(shadersys::fx::ShaderCullSide::Back, "BACK");
            archive.field(shadersys::fx::ShaderCullSide::Front, "FRONT");
        }
    };
} // namespace ionengine::core