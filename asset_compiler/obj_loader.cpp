// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset_compiler/obj_loader.h>
#include <lib/algorithm.h>

using namespace ionengine;

bool OBJLoader::parse(std::span<char8_t const> const data) {

    std::string_view const buffer(reinterpret_cast<char const*>(data.data()), data.size());
    std::string_view str;

    uint64_t offset = 0;

    std::map<std::u8string, uint32_t> material_indices;

    uint32_t cur_object_index = 0;
    uint32_t cur_material_index = 0;
    
    for(size_t read_bytes = 0; (read_bytes = get_line(buffer, offset, str, '\n')) > 0; ) {

        // Object
        if(str.contains("o ")) {
            std::string_view substr;
            size_t substr_offset = 0;

            size_t substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Skip space

            substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Object name
            
            std::u8string object_name(reinterpret_cast<char8_t const*>(substr.data()), substr_read_bytes);

            cur_object_index = static_cast<uint32_t>(_data.objects.size());
            _data.objects.emplace_back(object_name);
        }

        // Vertices
        if(str.contains("v ")) {
            std::string_view substr;
            size_t substr_offset = 0;

            size_t substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Skip space

            // Vertices coordinates (float3)
            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {
                float coord = 0.0f;
                auto result = std::from_chars(substr.data(), substr.data() + substr.size(), coord);
                _data.vertices.emplace_back(coord);
            }
        }

        // Vertex textures
        if(str.contains("vt ")) {
            std::string_view substr;
            size_t substr_offset = 0;

            size_t substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Skip space

            // Vertex textures coordinates (float2)
            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {
                float coord = 0.0f;
                auto result = std::from_chars(substr.data(), substr.data() + substr.size(), coord);
                _data.uvs.emplace_back(coord);
            }  
        }

        // Vertex normals
        if(str.contains("vn ")) {
            std::string_view substr;
            size_t substr_offset = 0;

            size_t substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Skip space

            // Vertex normals coordinates
            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {
                float coord = 0.0f;
                auto result = std::from_chars(substr.data(), substr.data() + substr.size(), coord);
                _data.normals.emplace_back(coord);
            }
        }
        
        // Material
        if(str.contains("usemtl ")) {
            std::string_view substr;
            size_t substr_offset = 0;

            size_t substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Skip space

            substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Material name

            std::u8string material_name(reinterpret_cast<char8_t const*>(substr.data()), substr_read_bytes);

            auto it = material_indices.find(material_name);
            if(it != material_indices.end()) {
                cur_material_index = it->second;
            } else {
                cur_material_index = static_cast<uint32_t>(_data.objects[cur_object_index].materials.size());
                _data.objects[cur_object_index].materials.emplace_back(material_name);
                material_indices[material_name] = cur_material_index;
            }
        }

        // Faces
        if(str.contains("f ")) {

            std::string_view substr;
            size_t substr_offset = 0;

            size_t substr_read_bytes = get_line(str, substr_offset, substr, ' '); // Skip space

            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {

                std::string_view substr_2;
                size_t substr_offset_2 = 0;

                std::array<uint32_t, 3> indices;
                for(size_t substr_read_bytes_2 = 0, j = 0; (substr_read_bytes_2 = get_line(substr, substr_offset_2, substr_2, '/')) > 0; ++j) {
                    auto result = std::from_chars(substr_2.data(), substr_2.data() + substr_2.size(), indices[j]);
                    --indices[j]; // Reduce to 1 index because index start is 1 in OBJ format
                }

                _data.objects[cur_object_index].materials[cur_material_index].indices.emplace_back(indices[0], indices[1], indices[2]);
            }
        }
    }
    return true;
}
