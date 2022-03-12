// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>

namespace ionengine::renderer {

struct MaterialData {

    
};

struct ShaderData {
    std::u8string _name;
    ShaderFlags _flags;
    std::vector<char8_t> data;

    std::u8string const& name() const { return _name; }

    ShaderFlags flags() const { return _flags; }
};

struct TextureData {
    Dimension _dimension;
    Format _format;
    uint32_t _width;
    uint32_t _height;
    uint16_t _mip_levels;
    uint16_t _array_layers;
    Filter _filter;
    AddressMode _address_u;
    AddressMode _address_v;
    AddressMode _address_w;
    std::vector<char8_t> data;

    Dimension dimension() const { return _dimension; }

    Format format() const { return _format; }

    uint32_t width() const { return _width; }

    uint32_t height() const { return _height; }

    uint16_t mip_levels() const { return _mip_levels; }

    uint16_t array_layers() const { return _array_layers; }

    Filter filter() const { return _filter; }

    AddressMode address_u() const { return _address_u; }
    
    AddressMode address_v() const { return _address_v; }

    AddressMode address_w() const { return _address_w; }
};

struct MeshData {
    std::vector<float> positions;
    std::vector<float> uv_normals;
    std::vector<uint32_t> indices;

    inline static std::vector<VertexInputDesc> vertex_declaration = {
        VertexInputDesc { "POSITION", 0, Format::RGB32, 0, sizeof(float) * 3 },
        VertexInputDesc { "TEXCOORD", 1, Format::RG32, 1, sizeof(float) * 2 },
        VertexInputDesc { "NORMAL", 2, Format::RGB32, 1, sizeof(float) * 3 }
    };

    
};

}