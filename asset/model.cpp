// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/model.h>

using namespace ionengine::asset;

Model::Model(std::filesystem::path const& file_path) {

    auto attributes = std::vector<PrimitiveAttribute> {
        { VertexUsage::Position, VertexFormat::F32x3, 0 },
        { VertexUsage::Color0, VertexFormat::F32x2, 1 },
    };

    std::vector<float> triangle = {
        -0.5f, -0.5f, 0.0f, 0.4f, 0.3f, 0.3f,
        0.5f, -0.5f, 0.0f, 0.1f, 0.8f, 0.3f,
        0.0f,  0.5f, 0.0f, 0.2f, 0.3f, 0.7f
    };
    _data.resize(triangle.size() * sizeof(float));
    std::memcpy(_data.data(), triangle.data(), _data.size());

    HashedBuffer vertex_buffer(_data, sizeof(float) * 3 + sizeof(float) * 3);
    HashedBuffer index_buffer(_data, sizeof(float) * 3);

    Primitive primitive = { .vertex_buffer = std::move(vertex_buffer), .index_buffer = std::move(index_buffer), .material_index = 0 };

    _surfaces.resize(1);
    _surfaces[0].attributes = attributes;
    _surfaces[0].primitives.push_back(primitive);
}

std::span<Surface const> Model::surfaces() const {

    return _surfaces;
}

HashedBuffer::HashedBuffer(std::span<uint8_t const> const data, size_t const element_size)  {
    
    _data = data;
    _data_hash = std::hash<std::string_view>()(std::string_view(reinterpret_cast<char const*>(_data.data()), reinterpret_cast<char const*>(_data.data() + _data.size())));

    _count = static_cast<uint32_t>(data.size() / element_size);
}

uint64_t HashedBuffer::hash() const  {
    
    return _data_hash;
}

uint8_t const* HashedBuffer::data() const  {
    
    return _data.data();
}

size_t HashedBuffer::size() const  {
    
    return _data.size();
}

uint32_t HashedBuffer::count() const {

    return _count;
}
