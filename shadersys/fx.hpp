// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine::asset
{
    namespace fx
    {
        std::array<uint8_t, 4> constexpr Magic{'F', 'X', '1', '0'};

        enum class ShaderFormat : uint32_t
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

        enum class FillMode
        {
            Solid,
            Wireframe
        };

        enum class CullMode
        {
            Back,
            Front,
            None
        };

        struct VertexLayoutElementData
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

        struct VertexLayoutData
        {
            std::vector<VertexLayoutElementData> elements;
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

        struct OutputData
        {
            bool depthWrite;
            bool stencilWrite;
            CullMode cullMode;
            FillMode fillMode;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(depthWrite, "depthWrite");
                archive.property(stencilWrite, "stencilWrite");
                archive.property(cullMode, "cullMode");
                archive.property(fillMode, "fillMode");
            }
        };

        struct StageData
        {
            StageType type;
            uint32_t buffer;
            std::string entryPoint;
            std::optional<VertexLayoutData> vertexLayout;
            std::optional<OutputData> output;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(buffer, "buffer");
                archive.property(entryPoint, "entryPoint");
                archive.property(vertexLayout, "vertexLayout");
                archive.property(output, "output");
            }
        };

        struct HeaderData
        {
            std::string name;
            std::string description;
            std::string domain;
            std::string blend;
            std::vector<std::string> features;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(description, "description");
                archive.property(domain, "domain");
                archive.property(blend, "blend");
                archive.property(features, "features");
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

        struct ShaderData
        {
            HeaderData headerData;
            std::unordered_map<StageType, StageData> stages;
            std::unordered_map<uint32_t, std::vector<uint32_t>> permutations;
            std::vector<StructureData> structures;
            std::vector<BufferData> buffers;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(headerData, "header");
                archive.property(permutations, "permutations");
                archive.property(stages, "stages");
                archive.property(structures, "structures");
                archive.property(buffers, "buffers");
            }
        };
    } // namespace fx

    struct ShaderFile
    {
        std::array<uint8_t, fx::Magic.size()> magic;
        fx::ShaderFormat shaderFormat;
        fx::ShaderData shaderData;
        std::vector<uint8_t> blob;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.property(shaderFormat);
            archive.template with<core::serialize_ojson, core::serialize_ijson>(shaderData);
            archive.property(blob);
        }
    };
} // namespace ionengine::asset

namespace ionengine::core
{
    template <>
    struct serializable_enum<asset::fx::VertexFormat>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(asset::fx::VertexFormat::RGBA32_FLOAT, "RGBA32_FLOAT");
            archive.field(asset::fx::VertexFormat::RGBA32_SINT, "RGBA32_SINT");
            archive.field(asset::fx::VertexFormat::RGBA32_UINT, "RGBA32_UINT");
            archive.field(asset::fx::VertexFormat::RGB32_FLOAT, "RGB32_FLOAT");
            archive.field(asset::fx::VertexFormat::RGB32_SINT, "RGB32_SINT");
            archive.field(asset::fx::VertexFormat::RGB32_UINT, "RGB32_UINT");
            archive.field(asset::fx::VertexFormat::RG32_FLOAT, "RG32_FLOAT");
            archive.field(asset::fx::VertexFormat::RG32_SINT, "RG32_SINT");
            archive.field(asset::fx::VertexFormat::RG32_UINT, "RG32_UINT");
            archive.field(asset::fx::VertexFormat::R32_FLOAT, "R32_FLOAT");
            archive.field(asset::fx::VertexFormat::R32_SINT, "R32_SINT");
            archive.field(asset::fx::VertexFormat::R32_UINT, "R32_UINT");
        }
    };

    template <>
    struct serializable_enum<asset::fx::ElementType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(asset::fx::ElementType::Float4x4, "float4x4");
            archive.field(asset::fx::ElementType::Float3x3, "float3x3");
            archive.field(asset::fx::ElementType::Float2x2, "float2x2");
            archive.field(asset::fx::ElementType::Float4, "float4");
            archive.field(asset::fx::ElementType::Float3, "float3");
            archive.field(asset::fx::ElementType::Float2, "float2");
            archive.field(asset::fx::ElementType::Float, "float");
            archive.field(asset::fx::ElementType::Uint, "uint");
            archive.field(asset::fx::ElementType::Sint, "int");
            archive.field(asset::fx::ElementType::Bool, "bool");
        }
    };

    template <>
    struct serializable_enum<asset::fx::StageType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(asset::fx::StageType::Vertex, "VS");
            archive.field(asset::fx::StageType::Pixel, "PS");
            archive.field(asset::fx::StageType::Compute, "CS");
        }
    };

    template <>
    struct serializable_enum<asset::fx::CullMode>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(asset::fx::CullMode::None, "NONE");
            archive.field(asset::fx::CullMode::Back, "BACK");
            archive.field(asset::fx::CullMode::Front, "FRONT");
        }
    };

    template <>
    struct serializable_enum<asset::fx::FillMode>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(asset::fx::FillMode::Wireframe, "WIREFRAME");
            archive.field(asset::fx::FillMode::Solid, "SOLID");
        }
    };
} // namespace ionengine::core