// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/mesh.h>

using namespace ionengine::asset;

Mesh::Mesh(std::filesystem::path const& file_path, AssetManager& asset_manager) {

    std::filesystem::path extension = file_path.extension();

    if(extension == ".obj") {

        /*tinyobj::attrib_t attributes;
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
        }*/
    }

    auto attributes = std::vector<MeshAttribute> {
        { VertexUsage::Position, VertexFormat::F32x3, 0 },
        { VertexUsage::Color, VertexFormat::F32x2, 1 },
    };

    _mesh_attributes.resize(attributes.size());
    std::memcpy(_mesh_attributes.data(), attributes.data(), _mesh_attributes.size());

    std::vector<float> triangles = {
        -0.5f, -0.5f, 0.0f, 0.4f, 0.3f, 0.3f,
        0.5f, -0.5f, 0.0f, 0.1f, 0.8f, 0.3f,
        0.0f,  0.5f, 0.0f, 0.2f, 0.3f, 0.7f
    };

    _mesh_vertices.resize(triangles.size());
    std::memcpy(_mesh_vertices.data(), triangles.data(), _mesh_vertices.size());

    // Triangle Primitive

    auto surface = MeshSurface { 
        .vertices = lib::hash::BufferView<float>(_mesh_vertices), 
        .indices = lib::hash::BufferView<uint32_t>(_mesh_indices), 
        .material_index = 0 
    };

    _mesh_surfaces.emplace_back(surface);
}

MeshSurface const& Mesh::surface(uint32_t const index) const {

    return _mesh_surfaces.at(index);
}
