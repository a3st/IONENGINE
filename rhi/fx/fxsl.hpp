// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rhi::fx
{
    std::array<uint8_t, 4> constexpr Magic{'F', 'X', 'S', 'L'};

    struct Header
    {
        std::array<uint8_t, Magic.size()> magic;
        uint32_t length;
        uint32_t target;
    };

    struct ChunkHeader
    {
        uint32_t chunk_type;
        uint32_t chunk_length;
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
        ShaderElementType constant_type;
        int32_t structure;
    };

    struct ShaderStructureElementData
    {
        std::string name;
        ShaderElementType element_type;
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
        std::string entry_point;
    };

    struct ShaderTechniqueData
    {
        ShaderStageData vertex_stage;
        ShaderStageData pixel_stage;
        ShaderStageData compute_stage;
        bool depth_write;
        bool stencil_write;
        ShaderCullSide cull_side;
    };

    struct FXSLObject
    {
        ShaderTargetType target;
        ShaderTechniqueData technique;
        std::vector<ShaderStructureData> structures;
        std::vector<std::vector<uint8_t>> buffers;
    };

    class FXSL
    {
      public:
        auto load_binary_from_memory(std::span<uint8_t const> const data) -> std::optional<FXSLObject>;

        auto load_binary_from_file(std::filesystem::path const& file_path) -> std::optional<FXSLObject>;
    };
} // namespace ionengine::rhi::fx