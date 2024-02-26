#include "glb.hpp"
#include "math/vector.hpp"
#include "precompiled.h"
#include <tiny_gltf.h>

namespace ionengine
{
    auto GLBImporter::load(std::span<uint8_t const> const data) -> bool
    {
        tinygltf::TinyGLTF loader;
        tinygltf::Model model;

        bool result = loader.LoadBinaryFromMemory(&model, nullptr, nullptr, data.data(), data.size());

        if (!result)
        {
            return false;
        }

        for (auto const& mesh : model.meshes)
        {
            std::vector<Primitive> primitives;

            for (auto const& primitive : mesh.primitives)
            {
                std::span<uint8_t const> vertices;
                uint32_t vertex_count = 0;
                std::span<uint8_t const> normals;
                std::span<uint8_t const> uvs;

                for (auto const& attribute : primitive.attributes)
                {
                    auto const& accessor = model.accessors[attribute.second];
                    auto const& buffer_view = model.bufferViews[accessor.bufferView];
                    auto const& buffer = model.buffers[buffer_view.buffer].data;

                    if (attribute.first.compare("POSITION") == 0)
                    {
                        vertices =
                            std::span<uint8_t const>(buffer.data() + buffer_view.byteOffset, buffer_view.byteLength);
                        vertex_count = accessor.count;
                    }
                    else if (attribute.first.compare("NORMAL") == 0)
                    {
                        normals =
                            std::span<uint8_t const>(buffer.data() + buffer_view.byteOffset, buffer_view.byteLength);
                    }
                    else if (attribute.first.compare("TEXCOORD_0") == 0)
                    {
                        uvs = std::span<uint8_t const>(buffer.data() + buffer_view.byteOffset, buffer_view.byteLength);
                    }
                }

                struct StaticVertex
                {
                    math::Vector3f position;
                    math::Vector3f normal;
                    math::Vector2f uv;
                };

                auto out = Primitive{};
                out.vertices.resize(vertex_count * sizeof(StaticVertex));

                for (uint32_t const i : std::views::iota(0u, vertex_count))
                {
                    auto vertex = StaticVertex{};
                    {
                        auto conv = reinterpret_cast<float const*>(vertices.data());
                        vertex.position = math::Vector3f(conv[i * 3 + 0], conv[i * 3 + 1], conv[i * 3 + 2]);
                    }
                    {
                        auto conv = reinterpret_cast<float const*>(normals.data());
                        vertex.normal = math::Vector3f(conv[i * 3 + 0], conv[i * 3 + 1], conv[i * 3 + 2]);
                    }
                    {
                        auto conv = reinterpret_cast<float const*>(uvs.data());
                        vertex.uv = math::Vector2f(conv[i * 2 + 0], conv[i * 2 + 1]);
                    }
                    std::memcpy(out.vertices.data() + i * sizeof(StaticVertex), &vertex, sizeof(StaticVertex));
                }

                uint32_t index_count = 0;
                {
                    auto const& accessor = model.accessors[primitive.indices];
                    auto const& buffer_view = model.bufferViews[accessor.bufferView];
                    auto const& buffer = model.buffers[buffer_view.buffer].data;

                    index_count = accessor.count;
                    auto indices =
                        std::span<uint8_t const>(buffer.data() + buffer_view.byteOffset, buffer_view.byteLength);

                    out.indices.resize(index_count * sizeof(uint32_t));

                    switch (accessor.componentType)
                    {
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: {
                            auto conv = reinterpret_cast<uint32_t const*>(indices.data());

                            for (uint32_t const i : std::views::iota(0u, index_count))
                            {
                                std::memcpy(out.indices.data() + i * sizeof(uint32_t), &conv[i], sizeof(uint32_t));
                            }
                            break;
                        }
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
                            auto conv = reinterpret_cast<uint16_t const*>(indices.data());

                            for (uint32_t const i : std::views::iota(0u, index_count))
                            {
                                std::memcpy(out.indices.data() + i * sizeof(uint32_t), &conv[i], sizeof(uint32_t));
                            }
                            break;
                        }
                        case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
                            auto conv = reinterpret_cast<uint8_t const*>(indices.data());

                            for (uint32_t const i : std::views::iota(0u, index_count))
                            {
                                std::memcpy(out.indices.data() + i * sizeof(uint32_t), &conv[i], sizeof(uint32_t));
                            }
                            break;
                        }
                    }
                }

                primitives.emplace_back(std::move(out));
            }

            auto out = Mesh{};
            out.material_index = mesh.primitives[0].material;
            out.primitives = std::move(primitives);

            this->data.meshes.emplace_back(std::move(out));
        }
        return true;
    }
} // namespace ionengine