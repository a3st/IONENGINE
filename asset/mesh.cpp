// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/mesh.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::asset;

Surface::Surface(std::span<float> const vertices, std::span<uint32_t> const indices, uint32_t const material_index) :
    _vertices(vertices), _indices(indices), _material_index(material_index) {
}

uint64_t Surface::hash() const {
    return _vertices.hash() ^ _indices.hash();
}

uint32_t Surface::material_index() const {
    return _material_index;
}

void Surface::cache_entry(size_t const value) {
    _cache_entry = value;
}

size_t Surface::cache_entry() const {
    return _cache_entry;
}

std::span<float const> Surface::vertices() const {
    return std::span<float const>(_vertices.data(), _vertices.size());
}

std::span<uint32_t const> Surface::indices() const {
    return std::span<uint32_t const>(_indices.data(), _indices.size());
}

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

    auto surface = std::make_shared<Surface>(triangles, indices, 0);
    _surfaces.emplace_back(surface);

    _materials.resize(1);

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

std::span<std::shared_ptr<Surface>> Mesh::surfaces() {
    return _surfaces;
}

void Mesh::material(uint32_t const index, AssetPtr<Material> material) {
    _materials.at(index) = material;
}

AssetPtr<Material> Mesh::material(uint32_t const index) {
    return _materials.at(index);
}

uint32_t Mesh::materials_size() const {
    return static_cast<uint32_t>(_materials.size());
}