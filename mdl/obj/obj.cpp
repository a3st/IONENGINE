// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "obj.hpp"
#include "precompiled.h"
#include <tiny_obj_loader.h>

namespace ionengine::asset
{
    auto OBJImporter::loadFromFile(std::filesystem::path const& filePath) -> std::expected<ModelFile, core::error>
    {
        tinyobj::ObjReader reader;

        tinyobj::ObjReaderConfig const config{};
        if (!reader.ParseFromFile(filePath.string(), config))
        {
            return std::unexpected(core::error(core::error_code::eof, reader.Error()));
        }

        tinyobj::attrib_t const& attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t> const& shapes = reader.GetShapes();

        uint32_t materialIndex = 0;
        std::unordered_map<Vertex, uint32_t, VertexHasher> uniqueVertices{};
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        mdl::ModelData modelData{
            .vertexLayout = {
                .elements =
                    {mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RGB32_FLOAT, .semantic = "POSITION"},
                     mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RGB32_FLOAT, .semantic = "NORMAL"},
                     mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RG32_FLOAT, .semantic = "TEXCOORD"}},
                .size = 32}};
        std::basic_stringstream<uint8_t> modelBlob;

        for (auto const& shape : shapes)
        {
            indices.clear();

            // std::cout << "shape v " << shape.mesh.num_face_vertices.size() << std::endl;

            size_t offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
            {
                size_t const fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);
                for (size_t v = 0; v < fv; ++v)
                {
                    tinyobj::index_t const index = shape.mesh.indices[offset + v];
                    Vertex vertex{};

                    if (index.vertex_index >= 0)
                    {
                        tinyobj::real_t const vx = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 0];
                        tinyobj::real_t const vy = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 1];
                        tinyobj::real_t const vz = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 2];

                        vertex.position = math::Vec3f(vx, vy, vz);
                    }

                    if (index.normal_index >= 0)
                    {
                        tinyobj::real_t const nx = attrib.normals[3 * static_cast<size_t>(index.normal_index) + 0];
                        tinyobj::real_t const ny = attrib.normals[3 * static_cast<size_t>(index.normal_index) + 1];
                        tinyobj::real_t const nz = attrib.normals[3 * static_cast<size_t>(index.normal_index) + 2];

                        vertex.normal = math::Vec3f(nx, ny, nz);
                    }

                    if (index.texcoord_index >= 0)
                    {
                        tinyobj::real_t const tx = attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 0];
                        tinyobj::real_t const ty = attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 1];

                        vertex.uv = math::Vec2f(tx, ty);
                    }

                    if (uniqueVertices.count(vertex) == 0)
                    {
                        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                        vertices.emplace_back(vertex);
                    }

                    indices.emplace_back(uniqueVertices[vertex]);
                }

                offset += fv;
            }

            mdl::SurfaceData const surfaceData{.buffer = static_cast<uint32_t>(modelData.buffers.size()),
                                               .material = materialIndex,
                                               .indexCount = static_cast<uint32_t>(indices.size())};
            modelData.surfaces.emplace_back(std::move(surfaceData));

            mdl::BufferData const bufferData{.offset = static_cast<uint64_t>(modelBlob.tellp()),
                                             .size = indices.size() * sizeof(uint32_t)};
            modelData.buffers.emplace_back(std::move(bufferData));

            modelBlob.write(reinterpret_cast<uint8_t const*>(indices.data()), indices.size() * sizeof(uint32_t));

            materialIndex++;
        }

        modelData.materialCount = materialIndex;
        modelData.buffer = static_cast<uint32_t>(modelData.buffers.size());

        mdl::BufferData const bufferData{.offset = static_cast<uint64_t>(modelBlob.tellp()),
                                         .size = vertices.size() * sizeof(Vertex)};
        modelData.buffers.emplace_back(std::move(bufferData));

        modelBlob.write(reinterpret_cast<uint8_t const*>(vertices.data()), vertices.size() * sizeof(Vertex));

        return ModelFile{.magic = mdl::Magic,
                         .modelData = std::move(modelData),
                         .blob = {std::istreambuf_iterator<uint8_t>(modelBlob.rdbuf()), {}}};
    }
} // namespace ionengine::asset