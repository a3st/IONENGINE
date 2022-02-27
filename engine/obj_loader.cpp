// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/obj_loader.h>

using namespace ionengine;

bool ObjLoader::parse(std::span<char8_t> const data) {

    auto get_line = [](std::string_view const buffer, size_t& offset, std::string_view& line, char const delimeter) -> size_t {
        size_t read_bytes = 0;
        for(size_t i = offset; i < buffer.size(); ++i) {
            ++read_bytes;
            if(buffer[i] == delimeter) {
                line = std::string_view(buffer.data() + offset, buffer.data() + i);
                break;
            }
        }
        if(read_bytes + offset == buffer.size()) {
            line = std::string_view(buffer.data() + offset, buffer.data() + buffer.size());
        }
        offset += read_bytes;
        return read_bytes;
    };

    std::string_view buffer(reinterpret_cast<char*>(data.data()), data.size());
    std::string_view str;
    size_t offset = 0;

    auto mesh = ObjMesh {};
    
    for(size_t read_bytes = 0; (read_bytes = get_line(buffer, offset, str, '\n')) > 0; ) {

        // vertices
        if(str.contains("v ")) {
            std::string_view substr;
            size_t substr_offset = 0;
            size_t substr_read_bytes = 0;

            // v space
            substr_read_bytes = get_line(str, substr_offset, substr, ' ');

            // v coords
            std::array<float, 3> coords;
            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {
                auto result = std::from_chars(substr.data(), substr.data() + substr.size(), coords[i]);
            }
            mesh.vertices.emplace_back(Vector3f(coords[0], coords[1], coords[2]));
        }

        // vt
        if(str.contains("vt ")) {
            std::string_view substr;
            size_t substr_offset = 0;
            size_t substr_read_bytes = 0;

            // vt space
            substr_read_bytes = get_line(str, substr_offset, substr, ' ');

            // vt coords
            std::array<float, 2> coords;
            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {
                auto result = std::from_chars(substr.data(), substr.data() + substr.size(), coords[i]);
            }
            mesh.uvs.emplace_back(Vector2f(coords[0], coords[1]));
        }

        // normals
        if(str.contains("vn ")) {
            std::string_view substr;
            size_t substr_offset = 0;
            size_t substr_read_bytes = 0;

            // vn space
            substr_read_bytes = get_line(str, substr_offset, substr, ' ');

            // vn coords
            std::array<float, 3> coords;
            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {
                auto result = std::from_chars(substr.data(), substr.data() + substr.size(), coords[i]);
            }
            mesh.normals.emplace_back(Vector3f(coords[0], coords[1], coords[2]));
        }
        
        if(str.contains("f ")) {

            // TO DO
            std::string_view substr;
            size_t substr_offset = 0;
            size_t substr_read_bytes = 0;

            // f space
            substr_read_bytes = get_line(str, substr_offset, substr, ' ');

            // v coords
            std::array<uint32_t, 9> indices;

            for(size_t substr_read_bytes = 0, i = 0; (substr_read_bytes = get_line(str, substr_offset, substr, ' ')) > 0; ++i) {

                std::string_view substr_2;
                size_t substr_offset_2 = 0;
                size_t substr_read_bytes_2 = 0;

                for(size_t substr_read_bytes_2 = 0, j = 0; (substr_read_bytes_2 = get_line(substr, substr_offset_2, substr_2, '/')) > 0; ++j) {
                    auto result = std::from_chars(substr_2.data(), substr_2.data() + substr_2.size(), indices[i + (j * 3)]);
                }
            }

            mesh.indices.insert(mesh.indices.end(), indices.begin(), indices.end());
        }
    }

    _meshes.emplace_back(mesh);
    return true;
}
