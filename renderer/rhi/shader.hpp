// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

namespace shader_file {

inline std::array<uint8_t, 8> const SHADER_MAGIC{ 'S', 'H', 'A', 'D', 'E', 'R', '.', '1' };

enum class ElementType {
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

enum class ResourceType {
    Buffer,
    Texture_UAV
};

enum class ShaderStageType {
    Vertex,
    Pixel
};

enum class ShaderFileFlags {
    DXIL_Binary = 0,
    SPIRV_Binary = 1
};

struct Header {
    std::array<uint8_t, 8> magic;
    uint32_t length;
    uint32_t flags;
};

struct ChunkHeader {
    uint32_t chunk_type;
    uint32_t chunk_length;
};

struct VertexStageInput {
    ElementType element_type;
    std::string semantic;
};

struct ShaderStageData {
    uint32_t buffer;
    std::string entry_point;
    std::vector<VertexStageInput> inputs;
    uint32_t inputs_size_per_vertex;
};

struct ElementData {
    std::string name;
    ElementType element_type;
};

struct ResourceData {
    uint32_t binding;
    ResourceType resource_type;
    std::vector<ElementData> elements;
    uint32_t size;
};

auto get_element_type_by_string(std::string_view const element_type) -> ElementType;

auto get_resource_type_by_string(std::string_view const resource_type) -> ResourceType;

auto get_shader_stage_type_by_string(std::string_view const stage_type) -> ShaderStageType;

class ShaderFile {
public:

    ShaderFile(std::span<uint8_t const> const data_bytes);

    auto get_name() -> std::string_view {

        return shader_name;
    }

    auto get_exports() -> std::unordered_map<std::string, ResourceData> const& {

        return exports;
    }

    auto get_stages() -> std::unordered_map<ShaderStageType, ShaderStageData> const& {

        return stages;
    }

    auto get_buffer(uint32_t const index) -> std::span<uint8_t const> {

        return buffers[index];
    }

    auto get_flags() -> ShaderFileFlags {

        return flags;
    }

private:

    std::string shader_name;
    std::unordered_map<ShaderStageType, ShaderStageData> stages;
    std::unordered_map<std::string, ResourceData> exports;
    std::vector<std::vector<uint8_t>> buffers;
    ShaderFileFlags flags;
};

}

class Shader : public core::ref_counted_object {
public:

    virtual auto get_name() -> std::string_view = 0;
};

}

}

}