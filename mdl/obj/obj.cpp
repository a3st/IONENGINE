// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "obj.hpp"
#include "precompiled.h"

namespace ionengine::mdl
{
    auto OBJImporter::loadFromFile(std::filesystem::path const& filePath,
                                   std::string& errors) -> std::optional<ModelFile>
    {
        tinyobj::ObjReader objReader;

        tinyobj::ObjReaderConfig config{};
        if (!objReader.ParseFromFile(filePath.string(), config))
        {
            return std::nullopt;
        }

        return readOBJToModelFile(objReader, errors);
    }

    auto OBJImporter::loadFromBytes(std::span<uint8_t const> const dataBytes,
                                    std::string& errors) -> std::optional<ModelFile>
    {
        tinyobj::ObjReader objReader;

        tinyobj::ObjReaderConfig config{};
        if (!objReader.ParseFromString(std::string(reinterpret_cast<char const*>(dataBytes.data()), dataBytes.size()),
                                       {}, config))
        {
            return std::nullopt;
        }

        return readOBJToModelFile(objReader, errors);
    }

    auto OBJImporter::readOBJToModelFile(tinyobj::ObjReader const& reader,
                                         std::string& errors) -> std::optional<ModelFile>
    {
        tinyobj::attrib_t const& attrib = reader.GetAttrib();
        std::vector<tinyobj::shape_t> const& shapes = reader.GetShapes();

        uint32_t materialIndex = 0;
        std::unordered_map<Vertex, uint32_t, VertexHasher> uniqueVertices{};
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        std::basic_stringstream<uint8_t> streambuf;

        ModelData modelData{};

        for (auto const& shape : shapes)
        {
            indices.clear();

            std::cout << "shape v " << shape.mesh.num_face_vertices.size() << std::endl;

            size_t offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f)
            {
                size_t const fv = static_cast<size_t>(shape.mesh.num_face_vertices[f]);

                for (size_t v = 0; v < fv; ++v)
                {
                    tinyobj::index_t const index = shape.mesh.indices[offset + v];

                    tinyobj::real_t const vx = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 0];
                    tinyobj::real_t const vy = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 1];
                    tinyobj::real_t const vz = attrib.vertices[3 * static_cast<size_t>(index.vertex_index) + 2];

                    Vertex vertex{.position = math::Vec3f(vx, vy, vz)};

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
                        uniqueVertices[vertex] = static_cast<size_t>(vertices.size());
                        vertices.emplace_back(vertex);
                    }

                    indices.emplace_back(uniqueVertices[vertex]);
                }

                offset += fv;
            }

            mdl::SurfaceData surfaceData{.buffer = static_cast<uint32_t>(modelData.buffers.size()),
                                         .material = materialIndex,
                                         .indexCount = static_cast<uint32_t>(indices.size())};
            modelData.surfaces.emplace_back(std::move(surfaceData));

            mdl::BufferData bufferData{.offset = static_cast<uint64_t>(streambuf.tellp()),
                                       .size = indices.size() * sizeof(uint32_t)};
            modelData.buffers.emplace_back(std::move(bufferData));

            streambuf.write(reinterpret_cast<uint8_t const*>(indices.data()), indices.size() * sizeof(uint32_t));

            materialIndex++;
        }

        modelData.materialCount = materialIndex;
        modelData.buffer = static_cast<uint32_t>(modelData.buffers.size());

        mdl::VertexLayoutData vertexLayoutData{
            .elements = {mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RGB32_FLOAT, .semantic = "POSITION"},
                         mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RGB32_FLOAT, .semantic = "NORMAL"},
                         mdl::VertexLayoutElementData{.format = mdl::VertexFormat::RG32_FLOAT,
                                                      .semantic = "TEXCOORD0"}},
            .size = 32};
        modelData.vertexLayout = std::move(vertexLayoutData);

        mdl::BufferData bufferData{.offset = static_cast<uint64_t>(streambuf.tellp()),
                                   .size = vertices.size() * sizeof(Vertex)};
        modelData.buffers.emplace_back(std::move(bufferData));

        streambuf.write(reinterpret_cast<uint8_t const*>(vertices.data()), vertices.size() * sizeof(Vertex));

        return ModelFile{.magic = mdl::Magic,
                         .modelData = std::move(modelData),
                         .blob = {std::istreambuf_iterator<uint8_t>(streambuf.rdbuf()), {}}};
    }
} // namespace ionengine::mdl