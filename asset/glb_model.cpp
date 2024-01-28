// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "glb_model.hpp"
#include <simdjson.h>

using namespace ionengine;
using namespace glb;

GLBModel::GLBModel(std::span<uint8_t> const data_bytes) {

    std::basic_ispanstream<uint8_t> stream(data_bytes, std::ios::binary);

    auto header = Header {};
	stream.read((uint8_t*)&header, sizeof(Header));

    if(header.magic != GLB_MAGIC) {
        throw core::Exception("Trying to parse a corrupted GLB model");
    }

    std::vector<uint8_t> chunk_json_data;
    {
        ChunkHeader chunk_header;
        stream.read((uint8_t*)&chunk_header, sizeof(ChunkHeader));

        chunk_json_data.resize(chunk_header.chunk_length);
        stream.read((uint8_t*)chunk_json_data.data(), chunk_header.chunk_length);
    }

    simdjson::ondemand::parser parser;
    auto document = parser.iterate(chunk_json_data.data(), chunk_json_data.size(), chunk_json_data.size() + simdjson::SIMDJSON_PADDING);

    for(auto accessor : document["accessors"]) {
        auto data = AccessorData {
            .buffer_view = static_cast<uint32_t>(accessor["bufferView"].get_uint64().value()),
            .component_type = static_cast<ComponentType>(accessor["componentType"].get_uint64().value()),
            .count = static_cast<uint32_t>(accessor["count"].get_uint64().value()),
            .type = get_accesor_type_by_string(accessor["type"].get_string().value())
        };
        accessors.emplace_back(data);
    }

    for(auto buffer_view : document["bufferViews"]) {
        auto data = BufferViewData {
            .buffer = static_cast<uint32_t>(buffer_view["buffer"].get_uint64().value()),
            .length = buffer_view["byteLength"].get_uint64().value(),
            .offset = buffer_view["byteOffset"].get_uint64().value()
        };
        buffer_views.emplace_back(data);
    }

    for(auto mesh : document["meshes"]) {
        std::vector<PrimitiveData> primitives;

        for(auto primitive : mesh["primitives"]) {
            std::vector<AttributeData> attributes;
            
            for(auto attribute : primitive["attributes"].get_object()) {
                auto attribute_data = AttributeData {
                    .attrib_name = std::string(attribute.unescaped_key().value()),
                    .accessor = static_cast<uint32_t>(attribute.value().get_uint64().value())
                };
                attributes.emplace_back(attribute_data);
            }

            auto primitive_data = PrimitiveData {
                .attributes = std::move(attributes),
                .indices = static_cast<uint32_t>(primitive["indices"].get_uint64().value()),
                .material = primitive.find_field("material").error() == simdjson::error_code::SUCCESS ?
                    static_cast<uint32_t>(primitive["material"].get_uint64().value()) : 0
            };

            primitives.emplace_back(primitive_data);
        }

        auto mesh_data = MeshData {
            .name = std::string(mesh["name"].get_string().value()),
            .primitives = std::move(primitives)
        };

        meshes.emplace_back(mesh_data);
    }

    do {
        std::vector<uint8_t> chunk_buffer_data;
        {
            ChunkHeader chunk_header;
            stream.read((uint8_t*)&chunk_header, sizeof(ChunkHeader));

            chunk_buffer_data.resize(chunk_header.chunk_length);
            stream.read((uint8_t*)chunk_buffer_data.data(), chunk_header.chunk_length);
        }

        buffers.emplace_back(std::move(chunk_buffer_data));
    } while(header.length > stream.tellg());
}

auto GLBModel::get_accesor_type_by_string(std::string_view const type) -> AccessorType {

    AccessorType ret;
    if(type.find("MAT4") != std::string_view::npos) {
        ret = AccessorType::Mat4;
    } else if(type.find("MAT3") != std::string_view::npos) {
        ret = AccessorType::Mat3;
    } else if(type.find("MAT2") != std::string_view::npos) {
        ret = AccessorType::Mat2;
    } else if(type.find("VEC3") != std::string_view::npos) {
        ret = AccessorType::Vec3;
    } else if(type.find("VEC2") != std::string_view::npos) {
        ret = AccessorType::Vec2;
    } else if(type.find("SCALAR") != std::string_view::npos) {
        ret = AccessorType::Scalar;
    }
    return ret;
}