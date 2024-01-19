// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "model.hpp"
#include <xxhash/xxhash64.h>
#include "math/vector.hpp"

using namespace ionengine;

Model::Model(std::span<uint8_t> const data_bytes, ModelFormat const format) {
    
    switch(format) {
        case ModelFormat::GLB: {

            glb::GLBModel model(data_bytes);

            for(auto const& mesh : model.get_meshes()) {

                std::vector<PrimitiveData> primitives;
                std::vector<uint32_t> index_counts;
                
                for(auto const& primitive : mesh.primitives) {
                    
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
                            for(auto const& attribute : primitive.attributes) {
                                auto const& accessor = model.get_accessors()[attribute.accessor];
                                auto const& buffer_view = model.get_buffer_views()[accessor.buffer_view];

                                if(attribute.attrib_name.find("POSITION") != std::string::npos) {
                                    vertices = std::span<float const>(
                                        reinterpret_cast<float const*>(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset), 
                                        buffer_view.length / sizeof(float)
                                    );
                                    vertex_count = accessor.count;
                                } else if(attribute.attrib_name.find("NORMAL") != std::string::npos) {
                                    normals = std::span<float const>(
                                        reinterpret_cast<float const*>(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset), 
                                        buffer_view.length / sizeof(float)
                                    );
                                } else if(attribute.attrib_name.find("TEXCOORD_0") != std::string::npos) {
                                    texcoords_0 = std::span<float const>(
                                        reinterpret_cast<float const*>(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset), 
                                        buffer_view.length / sizeof(float)
                                    );
                                }
                            }
                        }

                        {
                            struct Vertex {
                                math::Vector3f position;
                                math::Vector3f normal;
                                math::Vector2f texcoord_0;
                            };

                            for(size_t i = 0; i < vertex_count; ++i) {
                                auto vertex = Vertex {
                                    .position = math::Vector3f(vertices[i * 3 + 0], vertices[i * 3 + 1], vertices[i * 3 + 2]),
                                    .normal = math::Vector3f(normals[i * 3 + 0], normals[i * 3 + 1], normals[i * 3 + 2]),
                                    .texcoord_0 = math::Vector2f(texcoords_0[i * 2 + 0], texcoords_0[i * 2 + 1])
                                };

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

                            switch(accessor.component_type) {
                                case glb::ComponentType::UInt: {
                                    auto indices = std::span<uint32_t const>(
                                        reinterpret_cast<uint32_t const*>(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset), 
                                        buffer_view.length / sizeof(uint32_t)
                                    );

                                    for(size_t i = 0; i < index_count; ++i) {
                                        uint32_t const index = indices[i];

                                        std::vector<uint8_t> into_buffer(sizeof(uint32_t));
                                        std::memcpy(into_buffer.data(), (uint8_t*)&index, sizeof(uint32_t));

                                        buffer.insert(buffer.end(), into_buffer.begin(), into_buffer.end());
                                    }
                                } break;
                                case glb::ComponentType::UShort: {
                                    auto indices = std::span<uint16_t const>(
                                        reinterpret_cast<uint16_t const*>(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset), 
                                        buffer_view.length / sizeof(uint16_t)
                                    );

                                    for(size_t i = 0; i < index_count; ++i) {
                                        uint32_t const index = indices[i];

                                        std::vector<uint8_t> into_buffer(sizeof(uint32_t));
                                        std::memcpy(into_buffer.data(), (uint8_t*)&index, sizeof(uint32_t));

                                        buffer.insert(buffer.end(), into_buffer.begin(), into_buffer.end());
                                    }
                                } break;
                                case glb::ComponentType::UByte: {
                                    auto indices = std::span<uint8_t const>(
                                        reinterpret_cast<uint8_t const*>(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset), 
                                        buffer_view.length
                                    );

                                    for(size_t i = 0; i < index_count; ++i) {
                                        uint32_t const index = indices[i];

                                        std::vector<uint8_t> into_buffer(sizeof(uint32_t));
                                        std::memcpy(into_buffer.data(), &index, sizeof(uint32_t));

                                        buffer.insert(buffer.end(), into_buffer.begin(), into_buffer.end());
                                    }
                                } break;
                                default: {
                                    throw core::Exception("Invalid component type for indices");
                                } break;
                            }

                            index_size = index_count * sizeof(uint32_t);
                        }
                    }

                    auto vertices = std::span<uint8_t const>(buffer.data(), vertex_size);
                    auto indices = std::span<uint8_t const>(buffer.data() + vertex_size, index_size);
                    uint64_t const hash = XXHash64::hash(buffer.data(), buffer.size(), 0);

                    auto primitive_data = PrimitiveData {
                        .buffer = std::move(buffer),
                        .vertices = vertices,
                        .indices = indices,
                        .hash = hash
                    };

                    primitives.emplace_back(primitive_data);
                    index_counts.emplace_back(index_count);
                }

                auto mesh_data = MeshData {
                    .primitives = std::move(primitives),
                    .index_counts = std::move(index_counts)
                };

                meshes.emplace_back(mesh_data);
            }

        } break;
        default: {
            throw core::Exception("Downloadable data in this format is not available");
        } break;
    }
}