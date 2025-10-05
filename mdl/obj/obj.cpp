// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

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
            return std::unexpected(core::error(reader.Error()));
        }

        tinyobj::attrib_t const& attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t> const& shapes = reader.GetShapes();

        uint32_t materialIndex = 0;
        std::unordered_map<Vertex, uint32_t, VertexHasher> uniqueVertices;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        std::vector<mdl::SurfaceData> modelSurfaces;
        std::vector<mdl::BufferData> modelBuffers;
        std::basic_stringstream<uint8_t> modelBlob;

        for (auto const& shape : shapes)
        {
            indices.clear();

            size_t offset = 0;
            for (uint32_t const f : std::views::iota(0u, shape.mesh.num_face_vertices.size()))
            {
                size_t const fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);
                for (uint32_t const v : std::views::iota(0u, fv))
                {
                    tinyobj::index_t const index = shape.mesh.indices[offset + v];
                    Vertex vertex{};

                    if (index.vertex_index >= 0)
                    {
                        tinyobj::real_t const vx = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 0];
                        tinyobj::real_t const vy = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 1];
                        tinyobj::real_t const vz = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 2];

                        vertex.position = core::Vec3f(vx, vy, vz);
                    }

                    if (index.normal_index >= 0)
                    {
                        tinyobj::real_t const nx = attrib.normals[3 * static_cast<size_t>(index.normal_index) + 0];
                        tinyobj::real_t const ny = attrib.normals[3 * static_cast<size_t>(index.normal_index) + 1];
                        tinyobj::real_t const nz = attrib.normals[3 * static_cast<size_t>(index.normal_index) + 2];

                        vertex.normal = core::Vec3f(nx, ny, nz);
                    }

                    if (index.texcoord_index >= 0)
                    {
                        tinyobj::real_t const tx = attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 0];
                        tinyobj::real_t const ty = attrib.texcoords[2 * static_cast<size_t>(index.texcoord_index) + 1];

                        vertex.uv = core::Vec2f(tx, ty);
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

            mdl::SurfaceData surfaceData{.buffer = static_cast<uint32_t>(modelSurfaces.size()),
                                         .material = materialIndex,
                                         .indexCount = static_cast<uint32_t>(indices.size())};
            modelSurfaces.emplace_back(std::move(surfaceData));

            mdl::BufferData bufferData{.offset = static_cast<uint64_t>(modelBlob.tellp()),
                                       .size = indices.size() * sizeof(uint32_t)};
            modelBuffers.emplace_back(std::move(bufferData));

            modelBlob.write(reinterpret_cast<uint8_t const*>(indices.data()), indices.size() * sizeof(uint32_t));

            materialIndex++;
        }

        uint32_t const materialCount = materialIndex;
        uint32_t const bufferIndex = static_cast<uint32_t>(modelBuffers.size());

        mdl::BufferData bufferData{.offset = static_cast<uint64_t>(modelBlob.tellp()),
                                   .size = vertices.size() * sizeof(Vertex)};
        modelBuffers.emplace_back(std::move(bufferData));

        modelBlob.write(reinterpret_cast<uint8_t const*>(vertices.data()), vertices.size() * sizeof(Vertex));

        mdl::ModelData modelData{
            .materialCount = materialCount,
            .buffer = bufferIndex,
            .vertexLayout = {.elements = {mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RGB32_FLOAT,
                                                                       .semantic = "POSITION"},
                                          mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RGB32_FLOAT,
                                                                       .semantic = "NORMAL"},
                                          mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RG32_FLOAT,
                                                                       .semantic = "TEXCOORD"}},
                             .size = 32},
            .surfaces = std::move(modelSurfaces),
            .buffers = std::move(modelBuffers)};
        return ModelFile{.magic = mdl::Magic,
                         .modelData = std::move(modelData),
                         .modelBlob = {std::istreambuf_iterator<uint8_t>(modelBlob.rdbuf()), {}}};
    }
} // namespace ionengine::asset