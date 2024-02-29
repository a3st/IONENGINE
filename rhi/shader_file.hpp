// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rhi::shader_file
{
    inline std::array<uint8_t, 8> const SHADER_MAGIC{'S', 'H', 'A', 'D', 'E', 'R', '.', '1'};

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
        Bool
    };

    enum class ResourceType
    {
        Uniform,
        NonUniform
    };

    enum class ShaderStageType
    {
        Vertex,
        Pixel
    };

    enum class ShaderFileFlags
    {
        DXIL = 0,
        SPIRV = 1
    };

    struct Header
    {
        std::array<uint8_t, 8> magic;
        uint32_t length;
        uint32_t flags;
    };

    struct ChunkHeader
    {
        uint32_t chunk_type;
        uint32_t chunk_length;
    };

    struct VertexStageInput
    {
        ElementType element_type;
        std::string semantic;
    };

    struct ShaderStageData
    {
        uint32_t buffer;
        std::string entry_point;
        std::vector<VertexStageInput> inputs;
        uint32_t inputs_size_per_vertex;
    };

    struct ElementData
    {
        std::string name;
        ElementType element_type;
        uint64_t offset;
    };

    struct ResourceData
    {
        uint32_t binding;
        ResourceType resource_type;
        std::vector<ElementData> elements;
        uint32_t size;
    };

    auto get_element_type_by_string(std::string_view const element_type) -> ElementType;

    auto get_resource_type_by_string(std::string_view const resource_type) -> ResourceType;

    auto get_shader_stage_type_by_string(std::string_view const stage_type) -> ShaderStageType;

    class ShaderFile
    {
      public:
        ShaderFile(std::span<uint8_t const> const data);

        auto get_name() const -> std::string_view;

        auto get_exports() const -> std::unordered_map<std::string, ResourceData> const&;

        auto get_stages() const -> std::unordered_map<ShaderStageType, ShaderStageData> const&;

        auto get_buffer(uint32_t const index) const -> std::span<uint8_t const>;

        auto get_flags() const -> ShaderFileFlags;

      private:
        std::string shader_name;
        std::unordered_map<ShaderStageType, ShaderStageData> stages;
        std::unordered_map<std::string, ResourceData> exports;
        std::vector<std::vector<uint8_t>> buffers;
        ShaderFileFlags flags;
    };
} // namespace ionengine::rhi::shader_file