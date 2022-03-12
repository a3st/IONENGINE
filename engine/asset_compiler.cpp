// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/asset_compiler.h>

#include <lib/algorithm.h>
#include <lib/math/vector.h>

//#include <engine/obj_loader.h>
//#include <engine/dds_loader.h>
#include <engine/hlsv_loader.h>

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

bool AssetCompiler::compile_shader(std::filesystem::path const& file_path, ShaderFile& shader_file) {

    std::vector<char8_t> buffer;
    
    size_t file_size = get_file_size(file_path, std::ios::binary);
    buffer.resize(file_size);

    if(!load_bytes_from_file(file_path, buffer, std::ios::binary)) {
        return false;
    }

    HLSVLoader hlsv_loader;

    if(!hlsv_loader.parse(buffer)) {
        return false;
    }

    shader_file.magic = SHADER_MAGIC;

    uint32_t shader_count = 0;

    for(auto it = hlsv_loader.begin(); it != hlsv_loader.end(); ++it) {

#ifdef IONENGINE_BACKEND_D3D12 
        if(it->flags & HLSVFlags::SPIRV) {
            continue;
        }
#endif

        ++shader_count;
    }

    shader_file.count = shader_count;
    shader_file.data = std::make_unique<renderer::ShaderData[]>(shader_count);

    uint32_t index = 0;

    for(auto it = hlsv_loader.begin(); it != hlsv_loader.end(); ++it) {

#ifdef IONENGINE_BACKEND_D3D12 
        if(it->flags & HLSVFlags::SPIRV) {
            continue;
        }
#endif

        shader_file.data[index]._name = it->name;

        if(it->name.contains(u8"_vertex")) {
            shader_file.data[index]._flags = renderer::ShaderFlags::Vertex;
        } else if(it->name.contains(u8"_pixel")) {
            shader_file.data[index]._flags = renderer::ShaderFlags::Pixel;
        } else if(it->name.contains(u8"_geometry")) {
            shader_file.data[index]._flags = renderer::ShaderFlags::Geometry;
        } else if(it->name.contains(u8"_domain")) {
            shader_file.data[index]._flags = renderer::ShaderFlags::Domain;
        } else if(it->name.contains(u8"_hull")) {
            shader_file.data[index]._flags = renderer::ShaderFlags::Hull;
        } else if(it->name.contains(u8"_compute")) {
            shader_file.data[index]._flags = renderer::ShaderFlags::Compute;
        }

        shader_file.data[index].data.resize(it->data.size());
        std::memcpy(shader_file.data[index].data.data(), it->data.data(), it->data.size());

        ++index;
    }

    return true;
}

bool AssetCompiler::compile(std::filesystem::path const& file_path, AssetFile& asset_file) {

    if(file_path.extension() == ".hlsv") {

        ShaderFile shader_file;

        if(!compile_shader(file_path, shader_file)) {
            std::cerr << std::format("[Error] AssetCompiler: The file '{}' cannot be compiled because it is corrupted", file_path.string()) << std::endl;
            return false;
        }

        asset_file.emplace<ShaderFile>(std::forward<ShaderFile>(shader_file));
        return true;
    }

    std::cerr << std::format("[Error] AssetCompiler: The file '{}' cannot be compiled because the format is not supported or not exists", file_path.string()) << std::endl;
    return false;
}

/*
bool AssetCompiler::compile(std::filesystem::path const& file_path, AssetFile& asset_file) {

    if(file_path.extension() == ".dds") {

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

        _file = mesh_file;

    } else if(file_path.extension() == ".shpk") {

    }

    return true;
}*/

size_t AssetCompiler::serialize_shader(std::vector<char8_t>& data, ShaderFile const& shader_file) {

    size_t total_shader_bytes = 0;

    for(uint32_t i = 0; i < shader_file.count; ++i) {
        
        total_shader_bytes = 
            total_shader_bytes + // Previous shader size
            sizeof(char8_t) * 48 + // std::u8string to char8_t[48]
            sizeof(uint32_t) + // ShaderFlags to uint32_t
            sizeof(size_t) + // ShaderCode size
            sizeof(char8_t) * shader_file.data[i].data.size() // ShaderCode bytes size
        ;
    }

    size_t const total_bytes = 
        sizeof(uint32_t) + // Magic size
        sizeof(uint32_t) + // Count size
        total_shader_bytes // ShaderData size
    ;

    data.resize(total_bytes);

    uint64_t offset = 0;

    std::memcpy(data.data(), &shader_file.magic, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    std::memcpy(data.data() + offset, &shader_file.count, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    for(uint32_t i = 0; i < shader_file.count; ++i) {
        std::memcpy(data.data() + offset, shader_file.data[i]._name.data(), shader_file.data[i]._name.size());
        offset += sizeof(char8_t) * 48;
        std::memcpy(data.data() + offset, &shader_file.data[i]._flags, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        size_t const data_size = shader_file.data[i].data.size();
        std::memcpy(data.data() + offset, &data_size, sizeof(size_t));
        offset += sizeof(size_t);
        std::memcpy(data.data() + offset, shader_file.data[i].data.data(), sizeof(char8_t) * data_size);
        offset += sizeof(char8_t) * data_size;
    }

    return total_bytes;
}

size_t AssetCompiler::serialize(std::vector<char8_t>& data, AssetFile const& asset_file) {

    size_t total_bytes = 0;

    auto asset_visitor = make_visitor(
        [&](ShaderFile const& shader_file) {
            total_bytes = serialize_shader(data, shader_file);
        },
        [&](MeshFile const& mesh_file) {
            // TODO!
        },
        [&](TextureFile const& texture_file) {
            // TODO!
        }
    );

    std::visit(asset_visitor, asset_file);
    return total_bytes;
}

bool AssetCompiler::deserialize_shader(std::vector<char8_t> const& data, ShaderFile& shader_file) {

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    if(read_bytes(data, offset, buffer, sizeof(SHADER_MAGIC)) > 0) {
        std::memcpy(&shader_file.magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    if(shader_file.magic != SHADER_MAGIC) {
        return false;
    }

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&shader_file.count, buffer.data(), buffer.size());
    } else {
        return false;
    }

    shader_file.data = std::make_unique<renderer::ShaderData[]>(shader_file.count);

    for(uint32_t i = 0; i < shader_file.count; ++i) {

        char8_t name[48];

        if(read_bytes(data, offset, buffer, sizeof(char8_t) * 48) > 0) {
            std::memcpy(name, buffer.data(), buffer.size());
        } else {
            return false;
        }

        shader_file.data[i]._name = std::u8string(name);
        
        if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
            std::memcpy(&shader_file.data[i]._flags, buffer.data(), buffer.size());
        } else {
            return false;
        }

        size_t data_size = 0; 

        if(read_bytes(data, offset, buffer, sizeof(size_t)) > 0) {
            std::memcpy(&data_size, buffer.data(), buffer.size());
        } else {
            return false;
        }

        shader_file.data[i].data.resize(data_size);

        if(read_bytes(data, offset, buffer, data_size) > 0) {
            std::memcpy(shader_file.data[i].data.data(), buffer.data(), buffer.size());
        } else {
            return false;
        }
    }

    return true;
}

bool AssetCompiler::deserialize(std::vector<char8_t> const& data, AssetFile& asset_file) {

    bool result = false;

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    uint32_t magic = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    if(magic == SHADER_MAGIC) {

        asset_file.emplace<ShaderFile>();
    }

    auto asset_visitor = make_visitor(
        [&](ShaderFile& shader_file) {
            result = deserialize_shader(data, shader_file);
        },
        [&](MeshFile& mesh_file) {
            // TODO!
        },
        [&](TextureFile& texture_file) {
            // TODO!
        }
    );

    std::visit(asset_visitor, asset_file);
    return result;
}
