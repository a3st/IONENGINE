// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/asset_compiler.h>

#include <lib/algorithm.h>

#include <engine/obj_loader.h>
#include <engine/dds_loader.h>
#include <engine/shpk_loader.h>

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

bool AssetCompiler::compile(std::filesystem::path const& file_path, AssetFile& asset_file) {

    if(file_path.extension() == ".dds") {

        /*auto dds_data = load_file(file, std::ios::binary);

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
        std::cout << ")" << std::endl;*/

    } else if(file_path.extension() == ".obj") {

        /*std::vector<char8_t> obj_data = load_file(file);

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
        std::memcpy(mesh_data.indices.data(), out_indices.data(), out_indices.size() * sizeof(uint32_t));

        auto mesh_file = MeshFile {};
        mesh_file.magic = static_cast<uint32_t>(Asset::MeshMagic);
        mesh_file.positions_count = static_cast<uint32_t>(mesh_data.positions.size());
        mesh_file.positions_offset = sizeof(MeshFile) - sizeof(renderer::MeshData);
        mesh_file.uv_normals_count = static_cast<uint32_t>(mesh_data.uv_normals.size());
        mesh_file.uv_normals_offset = mesh_file.positions_offset + static_cast<uint32_t>(mesh_data.positions.size()) * sizeof(float);
        mesh_file.indices_count = static_cast<uint32_t>(mesh_data.indices.size());
        mesh_file.indices_offset = mesh_file.uv_normals_offset + static_cast<uint32_t>(mesh_data.uv_normals.size()) * sizeof(float);
        mesh_file.data = mesh_data;

        _file = mesh_file;*/

    } else if(file_path.extension() == ".shpk") {

        std::vector<char8_t> buffer;

        size_t file_size = get_file_size(file_path);
        buffer.resize(file_size);

        if(!load_bytes_from_file(file_path, buffer, std::ios::binary)) {
            return false;
        }

        ShpkLoader shpk_loader;
        if(!shpk_loader.parse(buffer)) {
            return false;
        }

        shpk_loader.data()->
    }

    return true;
}

size_t AssetCompiler::serialize(std::vector<char8_t>& data) {

    uint64_t offset = 0;

    /*std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, MeshFile>) {
            size_t const header_size = sizeof(MeshFile) - sizeof(renderer::MeshData);
            size_t const positions_size = arg.data.positions.size() * sizeof(float);
            size_t const uv_normals_size = arg.data.uv_normals.size() * sizeof(float);
            size_t const indices_size = arg.data.indices.size() * sizeof(uint32_t);
            size_t const total_bytes = 
                header_size + // Header size
                positions_size + // MeshData positions size
                uv_normals_size + // MeshData uv normals size
                indices_size // MeshData indices size
            ;

            data.resize(total_bytes);
            std::memcpy(data.data(), &arg, header_size);
            offset += header_size;
            std::memcpy(data.data() + offset, arg.data.positions.data(), positions_size);
            offset += positions_size;
            std::memcpy(data.data() + offset, arg.data.uv_normals.data(), uv_normals_size);
            offset += uv_normals_size;
            std::memcpy(data.data() + offset, arg.data.indices.data(), indices_size);
            offset += indices_size;
        } else if constexpr (std::is_same_v<T, TextureFile>) {
            // TO DO
        } else if constexpr (std::is_same_v<T, ShaderFile>) {

        } else {
            static_assert(always_false_v<T>, "non-exhaustive visitor!");
        }
    }, _file);*/

    return offset;
}

bool AssetCompiler::deserialize(std::vector<char8_t> const& data) {

    /*auto read_bytes = [&](std::span<char8_t const> const src, uint64_t& offset, std::span<char8_t const>& dst, uint64_t const size) -> size_t {
        size_t read_bytes = 0;
        if(offset + size <= src.size()) {
            read_bytes = size;
            dst = std::span<char8_t const>(src.data() + offset, size);
        }
        offset += read_bytes;
        return read_bytes;
    };

    std::span<char8_t const> buffer;
    uint64_t offset = 0;

    uint32_t magic = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    offset = 0;

    switch(magic) {
        case static_cast<uint32_t>(Asset::MeshMagic): {
            auto model_file = MeshFile {};

            if(read_bytes(data, offset, buffer, sizeof(MeshFile) - sizeof(renderer::MeshData)) > 0) {
                std::memcpy(&model_file, buffer.data(), buffer.size());
            } else {
                return false;
            }

            if(read_bytes(data, offset, buffer, model_file.positions_count * sizeof(float)) > 0) {
                model_file.data.positions.resize(model_file.positions_count);
                std::memcpy(model_file.data.positions.data(), buffer.data(), buffer.size());
            } else {
                return false;
            }

            if(read_bytes(data, offset, buffer, model_file.uv_normals_count * sizeof(float)) > 0) {
                model_file.data.uv_normals.resize(model_file.uv_normals_count);
                std::memcpy(model_file.data.uv_normals.data(), buffer.data(), buffer.size());
            } else {
                return false;
            }

            if(read_bytes(data, offset, buffer, model_file.indices_count * sizeof(uint32_t)) > 0) {
                model_file.data.indices.resize(model_file.indices_count);
                std::memcpy(model_file.data.indices.data(), buffer.data(), buffer.size());
            } else {
                return false;
            }

            std::cout << "AssetType: Model" << std::endl;
        } break;
        case static_cast<uint32_t>(Asset::TextureMagic): {

            std::cout << "AssetType: Texture" << std::endl;
        } break;
    }*/
    
    return true;
}
