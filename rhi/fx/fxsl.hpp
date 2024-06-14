// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serializable.hpp"

namespace ionengine::rhi::fx
{
    std::array<uint8_t, 4> constexpr Magic{'F', 'X', 'S', 'L'};

    struct Header
    {
        std::array<uint8_t, Magic.size()> magic;
        uint32_t length;
        uint32_t target;
    };

    enum class ChunkType : uint32_t
    {
        JSON,
        BIN
    };

    struct ChunkHeader
    {
        uint32_t chunkType;
        uint32_t chunkLength;
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

    enum class ShaderTargetType : uint32_t
    {
        DXIL,
        SPIRV
    };

    enum class ShaderCullSide
    {
        Back,
        Front,
        None
    };

    struct ShaderConstantData
    {
        std::string name;
        ShaderElementType constantType;
        int32_t structure;
    };

    struct ShaderStructureElementData
    {
        std::string name;
        ShaderElementType elementType;
        std::string semantic;
    };

    struct ShaderStructureData
    {
        std::string name;
        std::vector<ShaderStructureElementData> elements;
        uint32_t size;
    };

    struct ShaderStageData
    {
        int32_t buffer;
        std::string entryPoint;
    };

    struct ShaderTechniqueData
    {
        std::unordered_map<ShaderStageType, ShaderStageData> stages;
        bool depthWrite;
        bool stencilWrite;
        ShaderCullSide cullSide;
    };

    struct ShaderEffectData
    {
        ShaderTargetType target;
        ShaderTechniqueData technique;
        std::vector<ShaderConstantData> constants;
        std::vector<ShaderStructureData> structures;
        std::vector<std::vector<uint8_t>> buffers;
    };

    namespace internal
    {
        auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<rhi::fx::ShaderEffectData>;

        auto serialize(rhi::fx::ShaderEffectData const& object, std::basic_ostream<uint8_t>& stream) -> size_t;
    } // namespace internal
} // namespace ionengine::rhi::fx

namespace ionengine::core
{
    template <>
    class Serializable<rhi::fx::ShaderEffectData>
    {
      public:
        static auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<rhi::fx::ShaderEffectData>
        {
            return rhi::fx::internal::deserialize(stream);
        }

        static auto serialize(rhi::fx::ShaderEffectData const& object, std::basic_ostream<uint8_t>& stream) -> size_t
        {
            return rhi::fx::internal::serialize(object, stream);
        }
    };
} // namespace ionengine::core