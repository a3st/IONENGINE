// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/buffer.h>
#include <lib/exception.h>

using namespace ionengine::asset;

namespace validation_error {

std::string usage(VertexAttributeUsage const usage) {
    switch(usage) {
        case VertexAttributeUsage::Position: return "POSITION";
        case VertexAttributeUsage::Normal: return "NORMAL";
        case VertexAttributeUsage::TexCoord0: return "TEXCOORD0";
        case VertexAttributeUsage::TexCoord1: return "TEXCOORD1";
        case VertexAttributeUsage::TexCoord2: return "TEXCOORD2";
        case VertexAttributeUsage::Tangent: return "TANGENT";
        default: return "INVALID";
    }
}

}

uint8_t vertex_attribute_type_size(VertexAttributeType const type) {
    switch(type) {
        case VertexAttributeType::Float32: return sizeof(float);
        case VertexAttributeType::UInt32: return sizeof(uint32_t);
        case VertexAttributeType::UInt16: return sizeof(uint16_t);
        case VertexAttributeType::UInt8: return sizeof(uint8_t);
        default: return 0;
    }
};

VertexBuffer::VertexBuffer(uint32_t const size, std::span<VertexAttributeDesc const> const attributes, std::span<uint8_t const> const data) {

    _data.resize(data.size());
    std::memcpy(_data.data(), data.data(), sizeof(char8_t) * data.size());

    for(auto& attribute : attributes) {
        for(auto& other_attribute : attributes) {
            if(attribute.usage == other_attribute.usage) {
                throw ionengine::lib::Exception(
                    std::format("A duplicate of vertex attribute (usage: {}, slot: {}, offset: {})", 
                        validation_error::usage(attribute.usage), attribute.slot, attribute.offset)
                );
            } else if(attribute.slot == other_attribute.slot) {
                throw ionengine::lib::Exception(
                    std::format("A duplicate shader slot (usage: {}, slot: {}, offset: {})", 
                        validation_error::usage(attribute.usage), attribute.slot, attribute.offset)
                );
            }
        }
    }

    _sparse_attributes.fill(std::nullopt);
    uint8_t vertex_size = 0;

    for(auto& attribute : attributes) {

        auto vertex_attribute = VertexAttributeDesc {
            .usage = attribute.usage,
            .data_type = attribute.data_type,
            .size = attribute.size,
            .slot = attribute.slot,
            .offset = attribute.offset
        };

        _dense_attributes.emplace_back(vertex_attribute);
        _sparse_attributes[static_cast<size_t>(attribute.usage)] = vertex_attribute;

        vertex_size += attribute.size * vertex_attribute_type_size(attribute.data_type);
    }

    _data_hash = std::hash<std::string_view>()(
        std::string_view(reinterpret_cast<char*>(_data.data()), reinterpret_cast<char*>(_data.data() + _data.size()))
    );

    _vertex_count = size;
    _vertex_size = vertex_size;
}

uint64_t VertexBuffer::data_hash() const {

    return _data_hash;
}

uint32_t VertexBuffer::vertex_count() const {

    return _vertex_count;
}

uint8_t VertexBuffer::vertex_size() const {

    return _vertex_size;
}

std::span<uint8_t const> VertexBuffer::data() const {

    return _data;
}
