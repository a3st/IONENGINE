// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/mesh.h>
#include <asset/asset_manager.h>
#include <lib/math/vector.h>

using namespace ionengine;
using namespace ionengine::asset;

void ionengine::asset::obj_shape_calculate_tangents(tinyobj::attrib_t const& attributes, tinyobj::shape_t const& shape, std::vector<float>& tangents, std::vector<float>& bitangents) {

    for(size_t i = 0; i < shape.mesh.indices.size(); i += 3) {

        auto v0 = lib::math::Vector3f(
            attributes.vertices.at(3 * shape.mesh.indices[i + 0].vertex_index + 0),
            attributes.vertices.at(3 * shape.mesh.indices[i + 0].vertex_index + 1),
            attributes.vertices.at(3 * shape.mesh.indices[i + 0].vertex_index + 2)
        );

        auto v1 = lib::math::Vector3f(
            attributes.vertices.at(3 * shape.mesh.indices[i + 1].vertex_index + 0),
            attributes.vertices.at(3 * shape.mesh.indices[i + 1].vertex_index + 1),
            attributes.vertices.at(3 * shape.mesh.indices[i + 1].vertex_index + 2)
        );

        auto v2 = lib::math::Vector3f(
            attributes.vertices.at(3 * shape.mesh.indices[i + 2].vertex_index + 0),
            attributes.vertices.at(3 * shape.mesh.indices[i + 2].vertex_index + 1),
            attributes.vertices.at(3 * shape.mesh.indices[i + 2].vertex_index + 2)
        );

        auto uv0 = lib::math::Vector2f(
            attributes.texcoords.at(2 * shape.mesh.indices[i + 0].texcoord_index + 0),
            attributes.texcoords.at(2 * shape.mesh.indices[i + 0].texcoord_index + 1)
        );

        auto uv1 = lib::math::Vector2f(
            attributes.texcoords.at(2 * shape.mesh.indices[i + 1].texcoord_index + 0),
            attributes.texcoords.at(2 * shape.mesh.indices[i + 1].texcoord_index + 1)
        );

        auto uv2 = lib::math::Vector2f(
            attributes.texcoords.at(2 * shape.mesh.indices[i + 2].texcoord_index + 0),
            attributes.texcoords.at(2 * shape.mesh.indices[i + 2].texcoord_index + 1)
        );

        lib::math::Vector3f dt_v1 = v1 - v0;
        lib::math::Vector3f dt_v2 = v2 - v0;
        lib::math::Vector2f dt_uv1 = uv1 - uv0;
        lib::math::Vector2f dt_uv2 = uv2 - uv0;

        float r = 1.0f / (dt_uv1.x * dt_uv2.y - dt_uv1.y * dt_uv2.x);
        lib::math::Vector3f tangent = (dt_v1 * dt_uv2.y - dt_v2 * dt_uv1.y) * r;
        lib::math::Vector3f bitangent = (dt_v2 * dt_uv1.x - dt_v1 * dt_uv2.x) * r;

        tangents.emplace_back(tangent.x);
        tangents.emplace_back(tangent.y);
        tangents.emplace_back(tangent.z);

        tangents.emplace_back(tangent.x);
        tangents.emplace_back(tangent.y);
        tangents.emplace_back(tangent.z);

        tangents.emplace_back(tangent.x);
        tangents.emplace_back(tangent.y);
        tangents.emplace_back(tangent.z);

        bitangents.emplace_back(bitangent.x);
        bitangents.emplace_back(bitangent.y);
        bitangents.emplace_back(bitangent.z);

        bitangents.emplace_back(bitangent.x);
        bitangents.emplace_back(bitangent.y);
        bitangents.emplace_back(bitangent.z);

        bitangents.emplace_back(bitangent.x);
        bitangents.emplace_back(bitangent.y);
        bitangents.emplace_back(bitangent.z);
    }
}

struct Vertex {
    lib::math::Vector3f position;
    lib::math::Vector2f uv;
    lib::math::Vector3f normal;
    lib::math::Vector3f tangent;
    lib::math::Vector3f bitangent;

    bool operator==(Vertex const& other) const {
        return std::tie(position, uv, normal, tangent, bitangent) == std::tie(other.position, other.uv, other.normal, other.tangent, other.bitangent);
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
            hash<lib::math::Vector3f>()(other.bitangent)
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

        std::vector<float> tangents;
        std::vector<float> bitangents;

        obj_shape_calculate_tangents(attributes, shape, tangents, bitangents);

        for(size_t i = 0; i < shape.mesh.indices.size(); ++i) {

            auto vertex = Vertex {
                .position = lib::math::Vector3f(
                    attributes.vertices.at(3 * shape.mesh.indices[i].vertex_index + 0), 
                    attributes.vertices.at(3 * shape.mesh.indices[i].vertex_index + 1), 
                    attributes.vertices.at(3 * shape.mesh.indices[i].vertex_index + 2)
                ),
                .uv = lib::math::Vector2f(
                    attributes.texcoords.at(2 * shape.mesh.indices[i].texcoord_index + 0),
                    attributes.texcoords.at(2 * shape.mesh.indices[i].texcoord_index + 1)
                ),
                .normal = lib::math::Vector3f(
                    attributes.normals.at(3 * shape.mesh.indices[i].normal_index + 0),
                    attributes.normals.at(3 * shape.mesh.indices[i].normal_index + 1),
                    attributes.normals.at(3 * shape.mesh.indices[i].normal_index + 2)
                ),
                .tangent = lib::math::Vector3f(
                    tangents.at(3 * i + 0), 
                    tangents.at(3 * i + 1), 
                    tangents.at(3 * i + 2)
                ),
                .bitangent = lib::math::Vector3f(
                    bitangents.at(3 * i + 0), 
                    bitangents.at(3 * i + 1), 
                    bitangents.at(3 * i + 2)
                )
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

                vertices.emplace_back(vertex.bitangent.x);
                vertices.emplace_back(vertex.bitangent.y);
                vertices.emplace_back(vertex.bitangent.z);
            }

            indices.push_back(unique_vertices[vertex]);
        }

        auto surface = SurfaceData {
            .vertices = std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(vertices.data()), vertices.size() * sizeof(float)),
            .indices = std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(indices.data()), indices.size() * sizeof(uint32_t)),
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
