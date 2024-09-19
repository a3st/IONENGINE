// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine::rhi::fx
{
    std::array<uint8_t, 4> constexpr Magic{'F', 'X', 'S', 'L'};

    enum class ShaderAPIType : uint32_t
    {
        D3D12,
        Vulkan
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

    struct ShaderResourceData
    {
        std::string resourceName;
        ShaderElementType resourceType;
        std::optional<uint32_t> structure;
        std::optional<std::string> defaultValue;

        template <typename Archive>
        auto operator()(Archive& archive)
        {
            archive.property(resourceName, "name");
            archive.property(resourceType, "type");
            archive.property(structure, "structure");
            archive.property(defaultValue, "default");
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
        std::vector<ShaderResourceData> constants;
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
} // namespace ionengine::rhi::fx

namespace ionengine::core
{
    template <>
    struct serializable_enum<rhi::fx::ShaderElementType>
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
    struct serializable_enum<rhi::fx::ShaderStageType>
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
    struct serializable_enum<rhi::fx::ShaderCullSide>
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