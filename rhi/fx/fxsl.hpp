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
        Pixel
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

    struct ShaderConstantData {
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

    /*!
        \brief FXSLParser class for parse FXSL effects
    */
    class FXSLParser
    {
      public:
        /*!
            \brief Parse FXSL from given data binary buffer
            
            \param data data binary buffer
        */
        FXSLParser(std::span<uint8_t const> const data);

        /*!
            \brief Get FXSL target platform

            \return a shader target platform (DXIL or SPIR-V)
        */
        auto get_target() const -> ShaderTargetType;

        /*!
            \brief Get FXSL shader technique info

            \returns a shader technique info like using stages, culling or stencil/depth parameters
        */
        auto get_technique() const -> ShaderTechniqueData const&;

        /*!
            \brief Get FXSL shader structures

            \returns a shader structures
        */
        auto get_structures() const -> std::span<ShaderStructureData>;

        /*!
            \brief Get binary shader buffer for graphics API

            \returns a binary buffer that contains DXIL or SPIR-V data
        */
        auto get_buffer(uint32_t const index) const -> std::span<uint8_t const>;

      private:
        ShaderTargetType target;
        ShaderTechniqueData technique;
        std::vector<ShaderStructureData> structures;
        std::vector<std::vector<uint8_t>> buffers;
    };
} // namespace ionengine::rhi::fx