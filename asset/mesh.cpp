// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/mesh.h>
#include <asset/asset_manager.h>
#include <lib/math/vector.h>

using namespace ionengine;
using namespace ionengine::asset;

struct Vertex {
    lib::math::Vector3f position;
    lib::math::Vector2f uv;
    lib::math::Vector3f normal;
    lib::math::Vector3f tangent;
    lib::math::Vector3f color;

    bool operator==(Vertex const& other) const {
        return std::tie(position, uv, normal, tangent, color) == std::tie(other.position, other.uv, other.normal, other.tangent, other.color);
    }
};

namespace std {

template<>
struct hash<Vertex> {
    size_t operator()(Vertex const& other) const {
        return 
            hash<lib::math::Vector3f>()(other.position) ^ 
            hash<lib::math::Vector2f>()(other.uv) ^ 
            hash<lib::math::Vector3f>()(other.normal) ^
            hash<lib::math::Vector3f>()(other.tangent) ^
            hash<lib::math::Vector3f>()(other.color)
        ;
    }
};

}

Mesh::Mesh(tinyobj::attrib_t const& attributes, std::span<tinyobj::shape_t const> const shapes, std::span<tinyobj::material_t const> const materials) {

    uint32_t material_index = 0;

    for(auto const& shape : shapes) {

        std::unordered_map<Vertex, uint32_t> unique_vertices;

        std::vector<float> vertices;
        std::vector<uint32_t> indices;

        for(auto const& index : shape.mesh.indices) {

            auto vertex = Vertex {
                .position = lib::math::Vector3f(
                    attributes.vertices.at(3 * index.vertex_index + 0), 
                    attributes.vertices.at(3 * index.vertex_index + 1), 
                    attributes.vertices.at(3 * index.vertex_index + 2)
                ),
                .uv = lib::math::Vector2f(
                    attributes.texcoords.at(2 * index.texcoord_index + 0),
                    attributes.texcoords.at(2 * index.texcoord_index + 1)
                ),
                .normal = lib::math::Vector3f(
                    attributes.normals.at(3 * index.normal_index + 0),
                    attributes.normals.at(3 * index.normal_index + 1),
                    attributes.normals.at(3 * index.normal_index + 2)
                ),
                .tangent = lib::math::Vector3f(0.0f, 0.0f, 0.0f),
                .color = lib::math::Vector3f(0.5f, 1.0f, 0.2f)
            };

            if (unique_vertices.count(vertex) == 0) {
                unique_vertices[vertex] = static_cast<uint32_t>(vertices.size()) / 14;
                
                vertices.emplace_back(vertex.position.x);
                vertices.emplace_back(vertex.position.y);
                vertices.emplace_back(vertex.position.z);

                vertices.emplace_back(vertex.uv.x);
                vertices.emplace_back(vertex.uv.y);

                vertices.emplace_back(vertex.normal.x);
                vertices.emplace_back(vertex.normal.y);
                vertices.emplace_back(vertex.normal.z);

                vertices.emplace_back(vertex.tangent.x);
                vertices.emplace_back(vertex.tangent.y);
                vertices.emplace_back(vertex.tangent.z);

                vertices.emplace_back(vertex.color.x);
                vertices.emplace_back(vertex.color.y);
                vertices.emplace_back(vertex.color.z);
            }
            indices.push_back(unique_vertices[vertex]);
        }

        auto surface = SurfaceData {
            .vertices = lib::hash::Buffer<float>(vertices),
            .indices = lib::hash::Buffer<uint32_t>(indices),
            .material_index = material_index
        };

        _surfaces.emplace_back(surface);
        ++material_index;
    }

    _materials.resize(material_index);
}

lib::Expected<Mesh, lib::Result<MeshError>> Mesh::load_from_file(std::filesystem::path const& file_path) {

    std::filesystem::path extension = file_path.extension();

    if(extension == ".obj") {

        tinyobj::attrib_t attributes;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string warn;
        std::string err;

        std::string input_file = file_path.string();

        tinyobj::LoadObj(&attributes, &shapes, &materials, &warn, &err, input_file.c_str());

        return lib::Expected<Mesh, lib::Result<MeshError>>::ok(Mesh(attributes, shapes, materials));

    } else {

        return lib::Expected<Mesh, lib::Result<MeshError>>::error(
            lib::Result<MeshError> { 
                .errc = MeshError::ParseError,
                .message = "Invalid format"
            }
        );
    }
}

std::span<SurfaceData> Mesh::surfaces() {
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
