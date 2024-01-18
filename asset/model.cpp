// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "model.hpp"

using namespace ionengine;

Model::Model(std::span<uint8_t> const data_bytes, ModelFormat const format) {
    
    switch(format) {
        case ModelFormat::GLB: {

            glb::GLBModel model(data_bytes);

            for(auto const& mesh : model.get_meshes()) {

                std::vector<PrimitiveData> primitives;
                
                for(auto const& primitive : mesh.primitives) {

                    std::vector<uint8_t> buffer;
                    {
                        size_t buffer_size = 0;

                        // Calculate buffer size for attributes
                        for(auto const& attribute : primitive.attributes) {
                            buffer_size += model.get_buffer_views()[model.get_accessors()[attribute.accessor].buffer_view].length;
                        }
                        // Add index buffer at end
                        buffer_size += model.get_buffer_views()[model.get_accessors()[primitive.indices].buffer_view].length;

                        buffer.resize(buffer_size);
                    }

                    std::span<uint8_t> vertices;
                    std::span<uint8_t> indices;
                    {
                        std::basic_ospanstream<uint8_t> stream(buffer, std::ios::binary);

                        for(auto const& attribute : primitive.attributes) {
                            auto const& buffer_view = model.get_buffer_views()[model.get_accessors()[attribute.accessor].buffer_view];
                            stream.write(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset, buffer_view.length);
                        }

                        vertices = std::span<uint8_t>(buffer.data(), stream.tellp());
                        
                        auto const& buffer_view = model.get_buffer_views()[model.get_accessors()[primitive.indices].buffer_view];
                        stream.write(model.get_buffer(buffer_view.buffer).data() + buffer_view.offset, buffer_view.length);

                        indices = std::span<uint8_t>(buffer.data() + vertices.size(), stream.tellp());
                    }

                    auto primitive_data = PrimitiveData {
                        .buffer = std::move(buffer),
                        .vertices = vertices,
                        .indices = indices
                    };

                    primitives.emplace_back(primitive_data);
                }

                auto mesh_data = MeshData {
                    .primitives = std::move(primitives)
                };

                meshes.emplace_back(mesh_data);
            }

        } break;
        default: {
            throw core::Exception("Downloadable data in this format is not available");
        } break;
    }
}