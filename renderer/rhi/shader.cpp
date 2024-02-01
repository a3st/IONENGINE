// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/exception.hpp"
#include "precompiled.h"
#include "shader.hpp"
#include <simdjson.h>

namespace ionengine::renderer::rhi::shader_file
{
    auto get_element_type_by_string(std::string_view const element_type) -> ElementType
    {
        ElementType ret{};
        if (element_type.compare("FLOAT4x4") == 0)
        {
            ret = ElementType::Float4x4;
        }
        else if (element_type.compare("FLOAT3x3") == 0)
        {
            ret = ElementType::Float3x3;
        }
        else if (element_type.compare("FLOAT2x2") == 0)
        {
            ret = ElementType::Float2x2;
        }
        else if (element_type.compare("FLOAT4") == 0)
        {
            ret = ElementType::Float4;
        }
        else if (element_type.compare("FLOAT3") == 0)
        {
            ret = ElementType::Float3;
        }
        else if (element_type.compare("FLOAT2") == 0)
        {
            ret = ElementType::Float2;
        }
        else if (element_type.compare("FLOAT") == 0)
        {
            ret = ElementType::Float;
        }
        else if (element_type.compare("UINT") == 0)
        {
            ret = ElementType::Uint;
        }
        else if (element_type.compare("BOOL") == 0)
        {
            ret = ElementType::Bool;
        }
        return ret;
    }

    auto get_resource_type_by_string(std::string_view const resource_type) -> ResourceType
    {
        ResourceType ret{};
        if (resource_type.compare("BUFFER") == 0)
        {
            ret = ResourceType::Buffer;
        }
        else if (resource_type.compare("NON_BUFFER") == 0)
        {
            ret = ResourceType::NonBuffer;
        }
        return ret;
    }

    auto get_shader_stage_type_by_string(std::string_view const stage_type) -> ShaderStageType
    {

        ShaderStageType ret{};
        if (stage_type.compare("VERTEX_SHADER") == 0)
        {
            ret = ShaderStageType::Vertex;
        }
        else if (stage_type.compare("PIXEL_SHADER") == 0)
        {
            ret = ShaderStageType::Pixel;
        }
        return ret;
    }

    ShaderFile::ShaderFile(std::span<uint8_t const> const data_bytes)
    {
        std::basic_ispanstream<uint8_t> stream(
            std::span<uint8_t>(const_cast<uint8_t*>(data_bytes.data()), data_bytes.size()), std::ios::binary);

        Header header;
        stream.read((uint8_t*)&header, sizeof(Header));

        if (std::memcmp(header.magic.data(), SHADER_MAGIC.data(), sizeof(SHADER_MAGIC)) != 0)
        {
            throw core::Exception("Trying to parse a corrupted shader");
        }

        flags = static_cast<ShaderFileFlags>(header.flags);

        std::vector<uint8_t> chunk_json_data;
        {
            ChunkHeader chunk_header;
            stream.read((uint8_t*)&chunk_header, sizeof(ChunkHeader));

            chunk_json_data.resize(chunk_header.chunk_length);
            stream.read(chunk_json_data.data(), chunk_json_data.size());
        }

        simdjson::ondemand::parser parser;
        auto document = parser.iterate(chunk_json_data.data(), chunk_json_data.size(),
                                       chunk_json_data.size() + simdjson::SIMDJSON_PADDING);

        shader_name = document["shaderName"].get_string().value();

        for (auto stage : document["stages"].get_object())
        {
            std::vector<VertexStageInput> inputs;
            uint32_t inputs_size_per_vertex = 0;

            ShaderStageType stage_type = get_shader_stage_type_by_string(stage.unescaped_key().value());
            if (stage_type == ShaderStageType::Vertex)
            {
                for (auto input : stage.value()["inputs"])
                {
                    auto input_data =
                        VertexStageInput{.element_type = get_element_type_by_string(input["type"].get_string().value()),
                                         .semantic = std::string(input["semantic"].get_string().value())};
                    inputs.emplace_back(input_data);
                }
                inputs_size_per_vertex = static_cast<uint32_t>(stage.value()["inputsSizePerVertex"].get_uint64());
            }

            auto stage_data =
                ShaderStageData{.buffer = static_cast<uint32_t>(stage.value()["buffer"].get_uint64()),
                                .entry_point = std::string(stage.value()["entryPoint"].get_string().value()),
                                .inputs = std::move(inputs),
                                .inputs_size_per_vertex = inputs_size_per_vertex};

            stages.emplace(stage_type, stage_data);
        }

        for (auto resource : document["exports"].get_object())
        {
            std::vector<ElementData> elements;
            uint32_t size = 0;

            ResourceType resource_type = get_resource_type_by_string(resource.value()["type"].get_string().value());
            if (resource_type == ResourceType::Buffer)
            {
                for (auto element : resource.value()["elements"])
                {
                    auto input_data =
                        ElementData{.name = std::string(element["name"].get_string().value()),
                                    .element_type = get_element_type_by_string(element["type"].get_string().value())};
                    elements.emplace_back(input_data);
                }
                size = static_cast<uint32_t>(resource.value()["sizeInBytes"].get_uint64());
            }

            auto resource_data =
                ResourceData{.binding = static_cast<uint32_t>(resource.value()["binding"].get_uint64()),
                             .resource_type = resource_type,
                             .elements = std::move(elements),
                             .size = size};

            exports.emplace(resource.unescaped_key().value(), resource_data);
        }

        do
        {
            std::vector<uint8_t> chunk_buffer_data;
            {
                ChunkHeader chunk_header;
                stream.read((uint8_t*)&chunk_header, sizeof(ChunkHeader));

                chunk_buffer_data.resize(chunk_header.chunk_length);
                stream.read(chunk_buffer_data.data(), chunk_buffer_data.size());
            }
            buffers.emplace_back(chunk_buffer_data);
        } while (header.length > stream.tellg());
    }
} // namespace ionengine::renderer::rhi::shader_file