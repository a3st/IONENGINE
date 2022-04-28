// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/model.h>

#include <tinyobjloader/tiny_obj_loader.h>

using namespace ionengine::asset;

Model::Model(std::filesystem::path const& file_path) {

    std::filesystem::path extension = file_path.extension();

    if(extension == ".obj") {

        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        std::string input_file = file_path.string();

        tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, input_file.c_str());

        if(!err.empty()) {
            std::cerr << err << std::endl;
        }

        

        for(auto& shape : shapes) {
            std::cout << shape.name << std::endl;
            shape.mesh.
        }
    }

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

    lib::hash::BufferView vertex_buffer(_data);
    lib::hash::BufferView index_buffer(_data);

    auto primitive = Primitive { 
        .vertex_buffer = std::move(vertex_buffer), 
        .index_buffer = std::move(index_buffer), 
        .vertex_count = 3,
        .index_count = 0,
        .material_index = 0 
    };

    _surfaces.resize(1);
    _surfaces[0].attributes = attributes;
    _surfaces[0].primitives.push_back(primitive);
}

std::span<Surface const> Model::surfaces() const {

    return _surfaces;
}
