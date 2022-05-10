// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/mesh.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::asset;

Mesh::Mesh() {

    _attributes.emplace_back(VertexUsage::Position, VertexFormat::F32x3, 0);
    _attributes.emplace_back(VertexUsage::Color, VertexFormat::F32x2, 1);

    std::vector<float> triangles = {
        -0.5f, -0.5f, 0.0f, 0.4f, 0.3f, 0.3f,
        0.5f, -0.5f, 0.0f, 0.1f, 0.8f, 0.3f,
        0.0f,  0.5f, 0.0f, 0.2f, 0.3f, 0.7f
    };

    std::vector<uint32_t> indices = {
        0, 1, 2, 
        3, 4, 5,
        6, 7, 8
    };

    auto surface = Surface {
        .vertices = lib::hash::Buffer<float>(triangles),
        .indices = lib::hash::Buffer<uint32_t>(indices),
        .material_index = 0
    };
    _surfaces.emplace_back(surface);

    /*
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
        }
    }
    */
}

lib::Expected<Mesh, lib::Result<MeshError>> Mesh::load_from_file(std::filesystem::path const& file_path) {

    std::filesystem::path extension = file_path.extension();

    if(extension == ".obj") {

    } else {

        return lib::Expected<Mesh, lib::Result<MeshError>>::error(
            lib::Result<MeshError> { 
                .errc = MeshError::ParseError,
                .message = "Invalid format"
            }
        );
    }

    return lib::Expected<Mesh, lib::Result<MeshError>>::ok(Mesh());   
}

std::span<Surface const> Mesh::surfaces() const {
    return _surfaces;
}

std::span<Surface> Mesh::surfaces() {
    return _surfaces;
}
