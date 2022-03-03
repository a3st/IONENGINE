// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/asset_compiler.h>

#include <engine/obj_loader.h>
#include <engine/dds_loader.h>

using namespace ionengine;

struct ObjVertex {
    Vector3f position;
    Vector2f uv;
    Vector3f normal;

    bool operator==(ObjVertex const& other) const {
        return std::tie(position, uv, normal) == std::tie(other.position, other.uv, other.normal);
    }
};

namespace std {
    template<> struct hash<ObjVertex> {
        size_t operator()(ObjVertex const& other) const {
            return hash<Vector3f>()(other.position) ^ hash<Vector2f>()(other.uv) ^ hash<Vector3f>()(other.normal);
        }
    };
}

bool AssetCompiler::compile(std::filesystem::path const& file) {

    auto load_file = [&](std::filesystem::path const& path, std::ios::openmode const ios = std::ios::beg) -> std::vector<char8_t> {
        std::ifstream ifs(path, ios);
        if(!ifs.is_open()) {
            throw std::runtime_error("Can't open file!");
        }
        ifs.seekg(0, std::ios::end);
        size_t size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char8_t> buf(size);
        ifs.read(reinterpret_cast<char*>(buf.data()), buf.size());
        return buf;
    };

    if(file.extension() == ".dds") {

        auto dds_data = load_file(file, std::ios::binary);

        DDSLoader dds_loader;
        if(!dds_loader.parse(dds_data)) {
            return false;
        }

        auto& header = dds_loader.data()->header;
        std::cout << std::format("Texture DDS (width: {}, height: {}, mip_count: {}, ", header.width, header.height, header.mip_count);

        std::cout << std::format("pf_size: {}, pf_four_cc: {:#06x}", header.pf.size, header.pf.four_cc);

        if(header.pf.flags & static_cast<uint32_t>(DDSFlags::RGB)) {
            std::cout << ", flags: rgb, ";
        }

        if(header.pf.flags & static_cast<uint32_t>(DDSFlags::FourCC)) {
            std::cout << ", flags: four_cc, ";
            if(header.pf.four_cc == static_cast<uint32_t>(DDSFourCC::DXT1)) {
                std::cout << "format: dxt1, ";
            } else if(header.pf.four_cc == static_cast<uint32_t>(DDSFourCC::DXT2)) {
                std::cout << "format: dxt2, ";
            } else if(header.pf.four_cc == static_cast<uint32_t>(DDSFourCC::DXT3)) {
                std::cout << "format: dxt3, ";
            } else if(header.pf.four_cc == static_cast<uint32_t>(DDSFourCC::DXT4)) {
                std::cout << "format: dxt4, ";
            } else if(header.pf.four_cc == static_cast<uint32_t>(DDSFourCC::DXT5)) {
                std::cout << "format: dxt5, ";
            }
        }
        std::cout << ")" << std::endl;

    } else if(file.extension() == ".obj") {

        std::vector<char8_t> obj_data = load_file(file);

        std::unordered_map<ObjVertex, uint32_t> unique_vertices;

        std::vector<ObjVertex> out_vertices;
        std::vector<uint32_t> out_indices;

        ObjLoader obj_loader;
        if(!obj_loader.parse(std::span<char8_t>(reinterpret_cast<char8_t*>(obj_data.data()), obj_data.size()))) {
            return false;
        }

        for(auto it = obj_loader.begin(); it != obj_loader.end(); ++it) {
            for(auto& index : it->indices) {
                auto vertex = ObjVertex {
                    Vector3f(it->vertices[index.vertex].x, it->vertices[index.vertex].y, it->vertices[index.vertex].z),
                    Vector2f(it->uvs[index.uv].x, it->uvs[index.uv].y),
                    Vector3f(it->normals[index.normal].x, it->normals[index.normal].y, it->normals[index.normal].z)
                };

                if(unique_vertices.count(vertex) == 0) {
                    unique_vertices[vertex] = static_cast<uint32_t>(out_vertices.size());
                    out_vertices.emplace_back(vertex);
                }
                out_indices.emplace_back(unique_vertices[vertex]);
            }
        }

        auto mesh_data = renderer::MeshData {};
        
        mesh_data.positions.reserve(out_indices.size());
        mesh_data.uv_normals.reserve(out_indices.size());

        for(auto& vertex : out_vertices) {
            mesh_data.positions.emplace_back(vertex.position.x);
            mesh_data.positions.emplace_back(vertex.position.y);
            mesh_data.positions.emplace_back(vertex.position.z);

            mesh_data.uv_normals.emplace_back(vertex.uv.x);
            mesh_data.uv_normals.emplace_back(vertex.uv.y);

            mesh_data.uv_normals.emplace_back(vertex.normal.x);
            mesh_data.uv_normals.emplace_back(vertex.normal.y);
            mesh_data.uv_normals.emplace_back(vertex.normal.z);
        }

        mesh_data.indices.resize(out_indices.size());
        std::memcpy(mesh_data.indices.data(), out_indices.data(), out_indices.size());

        auto mesh_file = MeshFile {};
        mesh_file.magic = ((uint32_t)(uint8_t)'M') | ((uint32_t)(uint8_t)'E' << 8) | ((uint32_t)(uint8_t)'S' << 16) | ((uint32_t)(uint8_t)'H' << 24);
        mesh_file.positions_count = static_cast<uint32_t>(mesh_data.positions.size());
        mesh_file.positions_offset = 0;

        //_asset.data = mesh_data;
    }

    return true;
}
