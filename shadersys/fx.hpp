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

        enum class ElementType
        {
            Float4x4,
            Float3x3,
            Float2x2,
            Float4,
            Float3,
            Float2,
            Float,
            Uint4,
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
            None,
            Back,
            Front
        };

        struct StructureElementData
        {
            std::string name;
            ElementType type;
            std::optional<std::string> semantic;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(name, "name");
                archive.property(type, "type");
                archive.property(semantic, "semantic");
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
            std::optional<uint32_t> inputStructure;
            std::optional<OutputData> output;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(buffer, "buffer");
                archive.property(entryPoint, "entryPoint");
                archive.property(inputStructure, "inputStructure");
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

        struct PermutationData
        {
            std::vector<uint32_t> stages;
            std::vector<StructureData> structures;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(stages, "stages");
                archive.property(structures, "structures");
            }
        };

        struct ShaderData
        {
            HeaderData header;
            std::unordered_map<uint32_t, PermutationData> permutations;
            std::vector<StageData> stages;
            std::vector<BufferData> buffers;

            template <typename Archive>
            auto operator()(Archive& archive)
            {
                archive.property(header, "header");
                archive.property(permutations, "permutations");
                archive.property(stages, "stages");
                archive.property(buffers, "buffers");
            }
        };
    } // namespace fx

    struct ShaderFile
    {
        std::array<uint8_t, fx::Magic.size()> magic;
        fx::ShaderFormat shaderFormat;
        fx::ShaderData shaderData;
        std::vector<uint8_t> shaderBlob;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.property(shaderFormat);
            archive.template with<core::serialize_ojson, core::serialize_ijson>(shaderData);
            archive.property(shaderBlob);
        }
    };
} // namespace ionengine::asset

namespace ionengine::core
{
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
            archive.field(asset::fx::ElementType::Uint4, "uint4");
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