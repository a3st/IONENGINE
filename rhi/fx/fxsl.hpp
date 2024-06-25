// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serializable.hpp"

namespace ionengine::rhi::fx
{
    std::array<uint8_t, 4> constexpr Magic{'F', 'X', 'S', 'L'};

    enum class ShaderTargetType : uint32_t
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

    inline std::unordered_map<ShaderElementType, size_t> ShaderElementSize = {
        {ShaderElementType::Float4x4, 64},      {ShaderElementType::Float3x3, 36},
        {ShaderElementType::Float2x2, 16},      {ShaderElementType::Float4, 16},
        {ShaderElementType::Float3, 12},        {ShaderElementType::Float2, 8},
        {ShaderElementType::Float, 4},          {ShaderElementType::Bool, 4},
        {ShaderElementType::Uint, 4},           {ShaderElementType::SamplerState, 4},
        {ShaderElementType::ConstantBuffer, 4}, {ShaderElementType::StorageBuffer, 4},
        {ShaderElementType::Texture2D, 4}};

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
        int32_t structure;

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
        std::string semantic;

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

    struct ShaderTechniqueData
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
        ShaderTechniqueData technique;
        std::vector<ShaderConstantData> constants;
        std::vector<ShaderStructureData> structures;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(technique, "technique");
            archive.property(constants, "constants");
            archive.property(structures, "structures");
        }
    };

    struct ShaderEffectFile
    {
        std::array<uint8_t, Magic.size()> magic;
        ShaderTargetType target;
        ShaderEffectData effectData;
        std::vector<std::vector<uint8_t>> buffers;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(magic);
            archive.property(target);
            archive.with<core::serialize::OutputJSON, core::serialize::InputJSON>(effectData);
            archive.property(buffers);
        }
    };
} // namespace ionengine::rhi::fx

namespace ionengine::core
{
    template <>
    struct SerializableEnum<rhi::fx::ShaderElementType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(rhi::fx::ShaderElementType::Float4x4, "FLOAT4x4");
            archive.field(rhi::fx::ShaderElementType::Float3x3, "FLOAT3x3");
            archive.field(rhi::fx::ShaderElementType::Float2x2, "FLOAT2x2");
            archive.field(rhi::fx::ShaderElementType::Float4, "FLOAT4");
            archive.field(rhi::fx::ShaderElementType::Float3, "FLOAT3");
            archive.field(rhi::fx::ShaderElementType::Float2, "FLOAT2");
            archive.field(rhi::fx::ShaderElementType::Float, "FLOAT");
            archive.field(rhi::fx::ShaderElementType::Uint, "UINT");
            archive.field(rhi::fx::ShaderElementType::Bool, "BOOL");
            archive.field(rhi::fx::ShaderElementType::SamplerState, "SAMPLER_STATE");
            archive.field(rhi::fx::ShaderElementType::ConstantBuffer, "CONSTANT_BUFFER");
            archive.field(rhi::fx::ShaderElementType::StorageBuffer, "STORAGE_BUFFER");
            archive.field(rhi::fx::ShaderElementType::Texture2D, "TEXTURE2D");
        }
    };

    template <>
    struct SerializableEnum<rhi::fx::ShaderStageType>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(rhi::fx::ShaderStageType::Vertex, "vertexShader");
            archive.field(rhi::fx::ShaderStageType::Pixel, "pixelShader");
            archive.field(rhi::fx::ShaderStageType::Compute, "computeShader");
        }
    };

    template <>
    struct SerializableEnum<rhi::fx::ShaderCullSide>
    {
        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.field(rhi::fx::ShaderCullSide::None, "none");
            archive.field(rhi::fx::ShaderCullSide::Back, "back");
            archive.field(rhi::fx::ShaderCullSide::Front, "front");
        }
    };
} // namespace ionengine::core