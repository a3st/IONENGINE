// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "model.hpp"
#include "math/vector.hpp"
#include "precompiled.h"
#include <xxhash/xxhash64.h>

namespace ionengine
{
    Model::Model(renderer::Renderer& renderer) : renderer(&renderer)
    {
    }

    auto Model::load(std::span<uint8_t> const data_bytes, ModelFormat const format, bool const immediate) -> bool
    {
        switch (format)
        {
            case ModelFormat::GLB: {
                glb::GLBModel model(data_bytes);

                for (auto const& mesh : model.get_meshes())
                {
                    std::vector<core::ref_ptr<renderer::Primitive>> primitives;
                    std::vector<uint32_t> index_counts;

                    for (auto const& primitive : mesh.primitives)
                    {

                        std::vector<uint8_t> buffer;
                        uint32_t index_count = 0;
                        size_t vertex_size = 0;
                        size_t index_size = 0;
                        {
                            std::span<float const> vertices;
                            std::span<float const> normals;
                            std::span<float const> texcoords_0;
                            size_t vertex_count = 0;
                            {
                                for (auto const& attribute : primitive.attributes)
                                {
                                    auto const& accessor = model.get_accessors()[attribute.accessor];
                                    auto const& buffer_view = model.get_buffer_views()[accessor.buffer_view];

                                    if (attribute.attrib_name.find("POSITION") != std::string::npos)
                                    {
                                        vertices = std::span<float const>(
                                            reinterpret_cast<float const*>(model.get_buffer(buffer_view.buffer).data() +
                                                                           buffer_view.offset),
                                            buffer_view.length / sizeof(float));
                                        vertex_count = accessor.count;
                                    }
                                    else if (attribute.attrib_name.find("NORMAL") != std::string::npos)
                                    {
                                        normals = std::span<float const>(
                                            reinterpret_cast<float const*>(model.get_buffer(buffer_view.buffer).data() +
                                                                           buffer_view.offset),
                                            buffer_view.length / sizeof(float));
                                    }
                                    else if (attribute.attrib_name.find("TEXCOORD_0") != std::string::npos)
                                    {
                                        texcoords_0 = std::span<float const>(
                                            reinterpret_cast<float const*>(model.get_buffer(buffer_view.buffer).data() +
                                                                           buffer_view.offset),
                                            buffer_view.length / sizeof(float));
                                    }
                                }
                            }

                            {
                                struct Vertex
                                {
                                    math::Vector3f position;
                                    math::Vector3f normal;
                                    math::Vector2f texcoord_0;
                                };

                                for (uint32_t const i : std::views::iota(0u, vertex_count))
                                {
                                    auto vertex = Vertex{
                                        .position = math::Vector3f(vertices[i * 3 + 0], vertices[i * 3 + 1],
                                                                   vertices[i * 3 + 2]),
                                        .normal =
                                            math::Vector3f(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]),
                                        .texcoord_0 = math::Vector2f(texcoords_0[i * 2 + 0], texcoords_0[i * 2 + 1])};

                                    std::vector<uint8_t> into_buffer(sizeof(Vertex));
                                    std::memcpy(into_buffer.data(), &vertex, sizeof(Vertex));

                                    buffer.insert(buffer.end(), into_buffer.begin(), into_buffer.end());
                                }

                                vertex_size = vertex_count * sizeof(Vertex);
                            }

                            {
                                auto const& accessor = model.get_accessors()[primitive.indices];
                                auto const& buffer_view = model.get_buffer_views()[accessor.buffer_view];

                                index_count = accessor.count;

                                switch (accessor.component_type)
                                {
                                    case glb::ComponentType::Uint: {
                                        auto indices = std::span<uint32_t const>(
                                            reinterpret_cast<uint32_t const*>(
                                                model.get_buffer(buffer_view.buffer).data() + buffer_view.offset),
                                            buffer_view.length / sizeof(uint32_t));

                                        for (uint32_t const i : std::views::iota(0u, index_count))
                                        {
                                            uint32_t const index = indices[i];

                                            std::vector<uint8_t> into_buffer(sizeof(uint32_t));
                                            std::memcpy(into_buffer.data(), (uint8_t*)&index, sizeof(uint32_t));

                                            buffer.insert(buffer.end(), into_buffer.begin(), into_buffer.end());
                                        }
                                        break;
                                    }
                                    case glb::ComponentType::Ushort: {
                                        auto indices = std::span<uint16_t const>(
                                            reinterpret_cast<uint16_t const*>(
                                                model.get_buffer(buffer_view.buffer).data() + buffer_view.offset),
                                            buffer_view.length / sizeof(uint16_t));

                                        for (uint32_t const i : std::views::iota(0u, index_count))
                                        {
                                            uint32_t const index = indices[i];

                                            std::vector<uint8_t> into_buffer(sizeof(uint32_t));
                                            std::memcpy(into_buffer.data(), (uint8_t*)&index, sizeof(uint32_t));

                                            buffer.insert(buffer.end(), into_buffer.begin(), into_buffer.end());
                                        }
                                        break;
                                    }
                                    case glb::ComponentType::Ubyte: {
                                        auto indices = std::span<uint8_t const>(
                                            reinterpret_cast<uint8_t const*>(
                                                model.get_buffer(buffer_view.buffer).data() + buffer_view.offset),
                                            buffer_view.length);

                                        for (uint32_t const i : std::views::iota(0u, index_count))
                                        {
                                            uint32_t const index = indices[i];

                                            std::vector<uint8_t> into_buffer(sizeof(uint32_t));
                                            std::memcpy(into_buffer.data(), &index, sizeof(uint32_t));

                                            buffer.insert(buffer.end(), into_buffer.begin(), into_buffer.end());
                                        }
                                        break;
                                    }
                                    default: {
                                        throw core::Exception("Invalid component type for indices");
                                    }
                                }

                                index_size = index_count * sizeof(uint32_t);
                            }
                        }

                        auto vertices = std::span<uint8_t const>(buffer.data(), vertex_size);
                        auto indices = std::span<uint8_t const>(buffer.data() + vertex_size, index_size);
                        primitives.emplace_back(renderer->create_primitive(index_count, vertices, indices, immediate));
                    }

                    auto mesh_data = MeshData{.primitives = std::move(primitives)};
                    meshes.emplace_back(std::move(mesh_data));
                }
                break;
            }
            default: {
                throw core::Exception("Downloadable data in this format is not available");
            }
        }
        return true;
    }
} // namespace ionengine