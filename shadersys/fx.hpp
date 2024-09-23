// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine::shadersys::fx
{
    std::array<uint8_t, 4> constexpr Magic{'F', 'X', '1', '0'};

    enum class ShaderAPIType : uint32_t
    {
        DXIL,
        SPIRV
    };

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
        Bool,
        SamplerState,
        ConstantBuffer,
        StorageBuffer,
        Texture2D
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

    struct ShaderConstantData
    {
        std::string constantName;
        ShaderElementType constantType;
        uint32_t structure;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(constantName, "name");
            archive.property(constantType, "type");
            archive.property(structure, "structure");
        }
    };

    struct ShaderStructureElementData
    {
        std::string elementName;
        ShaderElementType elementType;
        std::optional<std::string> semantic;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(elementName, "name");
            archive.property(elementType, "type");
            archive.property(semantic, "semantic");
        }
    };

    struct ShaderStructureData
    {
        std::string structureName;
        std::vector<ShaderStructureElementData> elements;
        uint32_t size;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(structureName, "name");
            archive.property(elements, "elements");
            archive.property(size, "sizeInBytes");
        }
    };

    struct ShaderStageData
    {
        int32_t buffer;
        std::string entryPoint;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(buffer, "buffer");
            archive.property(entryPoint, "entryPoint");
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

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(header, "header");
            archive.property(output, "output");
            archive.property(constants, "constants");
            archive.property(structures, "structures");
        }
    };

    struct ShaderEffectFile
    {
        std::array<uint8_t, Magic.size()> magic;
        ShaderAPIType apiType;
        ShaderEffectData effectData;
        std::vector<std::vector<uint8_t>> buffers;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.property(apiType);
            archive.template with<core::serialize_ojson, core::serialize_ijson>(effectData);
            archive.property(buffers);
        }
    };
} // namespace ionengine::shadersys::fx

namespace ionengine::core
{
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
            archive.field(shadersys::fx::ShaderElementType::Bool, "BOOL");
            archive.field(shadersys::fx::ShaderElementType::SamplerState, "SAMPLER_STATE");
            archive.field(shadersys::fx::ShaderElementType::ConstantBuffer, "CONSTANT_BUFFER");
            archive.field(shadersys::fx::ShaderElementType::StorageBuffer, "STORAGE_BUFFER");
            archive.field(shadersys::fx::ShaderElementType::Texture2D, "TEXTURE2D");
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
            archive.field(shadersys::fx::ShaderCullSide::None, "none");
            archive.field(shadersys::fx::ShaderCullSide::Back, "back");
            archive.field(shadersys::fx::ShaderCullSide::Front, "front");
        }
    };
} // namespace ionengine::core