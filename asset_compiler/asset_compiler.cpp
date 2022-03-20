// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset_compiler/asset_compiler.h>

#include <lib/algorithm.h>
#include <lib/math/vector.h>

//#include <engine/dds_loader.h>
#include <asset_compiler/hlsv_loader.h>
#include <asset_compiler/obj_loader.h>

using namespace ionengine;
using namespace ionengine::tools;

bool AssetCompiler::compile_shader_package(std::filesystem::path const& file_path, ShaderPackageFile& shader_package_file) {

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

    shader_package_file.magic = SHADER_MAGIC;

    uint32_t shader_data_count = 0;

    for(auto it = hlsv_loader.begin(); it != hlsv_loader.end(); ++it) {

        ++shader_data_count;
    }

    for(auto it = hlsv_loader.begin(); it != hlsv_loader.end(); ++it) {

        renderer::ShaderFlags shader_flags = {};

        if(it->name.contains(u8"_vertex")) {
            shader_flags = renderer::ShaderFlags::Vertex;
        } else if(it->name.contains(u8"_pixel")) {
            shader_flags = renderer::ShaderFlags::Pixel;
        } else if(it->name.contains(u8"_geometry")) {
            shader_flags = renderer::ShaderFlags::Geometry;
        } else if(it->name.contains(u8"_domain")) {
            shader_flags = renderer::ShaderFlags::Domain;
        } else if(it->name.contains(u8"_hull")) {
            shader_flags = renderer::ShaderFlags::Hull;
        } else if(it->name.contains(u8"_compute")) {
            shader_flags = renderer::ShaderFlags::Compute;
        }

        shader_package_file.data.data[it->name] = renderer::ShaderPackageData::ShaderInfo { 
            .flags = shader_flags,
            .data = it->data
        };
    }

    return true;
}

struct ObjVertex {
    Vector3f position;
    Vector2f uv;
    Vector3f normal;

    bool operator==(ObjVertex const& other) const {
        return std::tie(position, uv, normal) == std::tie(other.position, other.uv, other.normal);
    }
};

namespace std {

template<>
struct hash<ObjVertex> {
    size_t operator()(ObjVertex const& other) const {
        return hash<Vector3f>()(other.position) ^ hash<Vector2f>()(other.uv) ^ hash<Vector3f>()(other.normal);
    }
};

}

bool AssetCompiler::compile_mesh_surface(std::filesystem::path const& file_path, MeshSurfaceFile& mesh_surface_file) {

    std::vector<char8_t> buffer;
    
    size_t file_size = get_file_size(file_path);
    buffer.resize(file_size);

    if(!load_bytes_from_file(file_path, buffer)) {
        return false;
    }

    OBJLoader obj_loader;
    
    if(!obj_loader.parse(buffer)) {
        return false;
    }

    std::set<std::u8string> unique_materials;

    for(auto& object : obj_loader.data().objects) {
        for(auto& material : object.materials) {
            unique_materials.emplace(material.name);
        }
    }

    uint32_t const material_count = static_cast<uint32_t>(unique_materials.size());

    std::unordered_map<ObjVertex, uint32_t> unique_vertices;
    std::vector<ObjVertex> vertices;

    mesh_surface_file.magic = MESH_SURFACE_MAGIC;
    mesh_surface_file.data.material_infos.resize(material_count);

    for(auto& object : obj_loader.data().objects) {

        for(size_t i = 0; i < object.materials.size(); ++i) {

            for(auto& index : object.materials[i].indices) {

                auto vertex = ObjVertex {
                    .position = Vector3f(obj_loader.data().vertices[3 * index.vertex + 0], obj_loader.data().vertices[3 * index.vertex + 1], obj_loader.data().vertices[3 * index.vertex + 2]),
                    .uv = Vector2f(obj_loader.data().uvs[2 * index.uv + 0], obj_loader.data().uvs[2 * index.uv + 1]),
                    .normal = Vector3f(obj_loader.data().normals[3 * index.normal + 0], obj_loader.data().normals[3 * index.normal + 1], obj_loader.data().normals[3 * index.normal + 2])
                };

                if(unique_vertices.count(vertex) == 0) {
                    unique_vertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.emplace_back(vertex);
                }

                mesh_surface_file.data.material_infos[i].name = object.materials[i].name;
                mesh_surface_file.data.material_infos[i].indices.emplace_back(unique_vertices[vertex]);
            }
        }
    }

    for(auto& vertex : vertices) {

        mesh_surface_file.data.positions.emplace_back(vertex.position.x);
        mesh_surface_file.data.positions.emplace_back(vertex.position.y);
        mesh_surface_file.data.positions.emplace_back(vertex.position.z);

        mesh_surface_file.data.uv_normals.emplace_back(vertex.uv.x);
        mesh_surface_file.data.uv_normals.emplace_back(vertex.uv.y);
        mesh_surface_file.data.uv_normals.emplace_back(vertex.normal.x);
        mesh_surface_file.data.uv_normals.emplace_back(vertex.normal.y);
        mesh_surface_file.data.uv_normals.emplace_back(vertex.normal.z);
    }

    return true;
}

bool AssetCompiler::compile(std::filesystem::path const& file_path, AssetFile& asset_file) {

    if(file_path.extension() == ".hlsv") {

        ShaderPackageFile shader_package_file;

        if(!compile_shader_package(file_path, shader_package_file)) {
            std::cerr << std::format("[Error] AssetCompiler: The file '{}' cannot be compiled because it is corrupted", file_path.string()) << std::endl;
            return false;
        }

        asset_file.emplace<ShaderPackageFile>(shader_package_file);
        return true;
    }

    if(file_path.extension() == ".obj") {

        MeshSurfaceFile mesh_surface_file;

        if(!compile_mesh_surface(file_path, mesh_surface_file)) {
            std::cerr << std::format("[Error] AssetCompiler: The file '{}' cannot be compiled because it is corrupted", file_path.string()) << std::endl;
            return false;
        }

        asset_file.emplace<MeshSurfaceFile>(mesh_surface_file);
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
    return true;
}*/

size_t AssetCompiler::serialize_shader_package(std::span<char8_t>* const data, ShaderPackageFile const& shader_package_file) {

    size_t total_bytes = 0;

    for(auto& [name, _data] : shader_package_file.data.data) {

        total_bytes = total_bytes +
            sizeof(char8_t) * 48 + // std::u8string to char8_t[48]
            sizeof(uint32_t) + // ShaderFlags to uint32_t
            sizeof(size_t) + // ShaderCode size
            sizeof(char8_t) * _data.data.size() // ShaderCode bytes size
        ;
    }

    total_bytes = total_bytes +
        sizeof(uint32_t) + // Magic size
        sizeof(uint32_t) // Count size
    ;

    if(!data) {
        return total_bytes;
    }

    uint64_t offset = 0;

    std::memcpy(data->data(), &shader_package_file.magic, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    uint32_t const shader_data_count = static_cast<uint32_t>(shader_package_file.data.data.size());
    std::memcpy(data->data() + offset, &shader_data_count, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    for(auto& [name, _data] : shader_package_file.data.data) {

        std::memcpy(data->data() + offset, name.data(), sizeof(char8_t) * std::max<size_t>(name.size(), 48));
        offset += sizeof(char8_t) * 48;

        std::memcpy(data->data() + offset, &_data.flags, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        size_t const shader_data_size = _data.data.size();
        std::memcpy(data->data() + offset, &shader_data_size, sizeof(size_t));
        offset += sizeof(size_t);

        std::memcpy(data->data() + offset, _data.data.data(), sizeof(char8_t) * shader_data_size);
        offset += sizeof(char8_t) * shader_data_size;
    }

    return total_bytes;
}

size_t AssetCompiler::serialize_mesh_surface(std::span<char8_t>* const data, MeshSurfaceFile const& mesh_surface_file) {

    size_t total_bytes = 0;

    for(auto& material_info : mesh_surface_file.data.material_infos) {

        total_bytes += total_bytes +
            sizeof(char8_t) * 48 + // std::u8string to char8_t[48]
            sizeof(uint32_t) + // Indices size
            sizeof(uint32_t) * material_info.indices.size() // Indices bytes size
        ;
    }

    total_bytes = total_bytes +
        sizeof(uint32_t) + // Magic size
        sizeof(uint32_t) + // Positions size
        sizeof(float) * mesh_surface_file.data.positions.size() + // Positions bytes size
        sizeof(uint32_t) + // Normals size
        sizeof(float) * mesh_surface_file.data.uv_normals.size() + // Normals bytes size
        sizeof(uint32_t) // Count size
    ;

    if(!data) {
        return total_bytes;
    }

    uint64_t offset = 0;

    std::memcpy(data->data(), &mesh_surface_file.magic, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    uint32_t const positions_count = static_cast<uint32_t>(mesh_surface_file.data.positions.size());
    std::memcpy(data->data() + offset, &positions_count, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    std::memcpy(data->data() + offset, mesh_surface_file.data.positions.data(), sizeof(float) * positions_count);
    offset += sizeof(float) * positions_count;

    uint32_t const uv_normals_count = static_cast<uint32_t>(mesh_surface_file.data.uv_normals.size());
    std::memcpy(data->data() + offset, &uv_normals_count, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    std::memcpy(data->data() + offset, mesh_surface_file.data.uv_normals.data(), sizeof(float) * uv_normals_count);
    offset += sizeof(float) * uv_normals_count;

    uint32_t const material_info_count = static_cast<uint32_t>(mesh_surface_file.data.material_infos.size());
    std::memcpy(data->data() + offset, &material_info_count, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    for(auto& material_info : mesh_surface_file.data.material_infos) {

        std::memcpy(data->data() + offset, material_info.name.data(), sizeof(char8_t) * std::max<size_t>(material_info.name.size(), 48));
        offset += sizeof(char8_t) * 48;

        uint32_t const indices_count = static_cast<uint32_t>(material_info.indices.size());
        std::memcpy(data->data() + offset, &indices_count, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::memcpy(data->data() + offset, material_info.indices.data(), sizeof(uint32_t) * indices_count);
        offset += sizeof(uint32_t) * indices_count;
    }

    return total_bytes;
}

size_t AssetCompiler::serialize(std::span<char8_t>* const data, AssetFile const& asset_file) {

    size_t total_bytes = 0;

    auto asset_visitor = make_visitor(
        [&](ShaderPackageFile const& file) {
            total_bytes = serialize_shader_package(data, file);
        },
        [&](MeshSurfaceFile const& file) {
            total_bytes = serialize_mesh_surface(data, file);
        },
        [&](TextureFile const& texture_file) {
            // TODO!
        }
    );

    std::visit(asset_visitor, asset_file);
    return total_bytes;
}

bool AssetCompiler::deserialize_shader_package(std::span<char8_t const> const data, ShaderPackageFile& shader_package_file) {

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&shader_package_file.magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    if(shader_package_file.magic != SHADER_MAGIC) {
        return false;
    }

    uint32_t shader_data_count = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&shader_data_count, buffer.data(), buffer.size());
    } else {
        return false;
    }

    for(uint32_t i = 0; i < shader_data_count; ++i) {

        char8_t name[48];

        if(read_bytes(data, offset, buffer, sizeof(char8_t) * 48) > 0) {
            std::memcpy(name, buffer.data(), buffer.size());
        } else {
            return false;
        }

        uint32_t shader_flags = 0;
        
        if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
            std::memcpy(&shader_flags, buffer.data(), buffer.size());
        } else {
            return false;
        }

        size_t shader_data_size = 0; 

        if(read_bytes(data, offset, buffer, sizeof(size_t)) > 0) {
            std::memcpy(&shader_data_size, buffer.data(), buffer.size());
        } else {
            return false;
        }

        std::vector<char8_t> shader_data;
        shader_data.resize(shader_data_size);

        if(read_bytes(data, offset, buffer, shader_data_size) > 0) {
            std::memcpy(shader_data.data(), buffer.data(), buffer.size());
        } else {
            return false;
        }

        shader_package_file.data.data[std::u8string(name)] = renderer::ShaderPackageData::ShaderInfo { 
            .flags = static_cast<renderer::ShaderFlags>(shader_flags),
            .data = std::move(shader_data)
        };
    }

    return true;
}

bool AssetCompiler::deserialize_mesh_surface(std::span<char8_t const> const data, MeshSurfaceFile& mesh_surface_file) {

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&mesh_surface_file.magic, buffer.data(), buffer.size());
    } else {
        return false;
    }

    if(mesh_surface_file.magic != MESH_SURFACE_MAGIC) {
        return false;
    }

    uint32_t positions_count = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&positions_count, buffer.data(), buffer.size());
    } else {
        return false;
    }

    mesh_surface_file.data.positions.resize(positions_count);

    if(read_bytes(data, offset, buffer, positions_count * sizeof(float)) > 0) {
        std::memcpy(mesh_surface_file.data.positions.data(), buffer.data(), buffer.size());
    } else {
        return false;
    }

    uint32_t uv_normals_count = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&uv_normals_count, buffer.data(), buffer.size());
    } else {
        return false;
    }

    mesh_surface_file.data.uv_normals.resize(uv_normals_count);

    if(read_bytes(data, offset, buffer, uv_normals_count * sizeof(float)) > 0) {
        std::memcpy(mesh_surface_file.data.uv_normals.data(), buffer.data(), buffer.size());
    } else {
        return false;
    }

    uint32_t material_info_count = 0;

    if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
        std::memcpy(&material_info_count, buffer.data(), buffer.size());
    } else {
        return false;
    }

    mesh_surface_file.data.material_infos.resize(material_info_count);

    for(auto& material_info : mesh_surface_file.data.material_infos) {

        material_info.name.resize(48);

        if(read_bytes(data, offset, buffer, sizeof(char8_t) * 48) > 0) {
            std::memcpy(material_info.name.data(), buffer.data(), buffer.size());
        } else {
            return false;
        }

        uint32_t indices_count = 0;

        if(read_bytes(data, offset, buffer, sizeof(uint32_t)) > 0) {
            std::memcpy(&indices_count, buffer.data(), buffer.size());
        } else {
            return false;
        }

        material_info.indices.resize(indices_count);

        if(read_bytes(data, offset, buffer, indices_count * sizeof(uint32_t)) > 0) {
            std::memcpy(material_info.indices.data(), buffer.data(), buffer.size());
        } else {
            return false;
        }
    }

    return true;
}

bool AssetCompiler::deserialize(std::span<char8_t const> const data, AssetFile& asset_file) {

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
        asset_file.emplace<ShaderPackageFile>();
    } else if(magic == MESH_SURFACE_MAGIC) {
        asset_file.emplace<MeshSurfaceFile>();
    }

    auto asset_visitor = make_visitor(
        [&](ShaderPackageFile& file) {
            result = deserialize_shader_package(data, file);
        },
        [&](MeshSurfaceFile& file) {
            result = deserialize_mesh_surface(data, file);
        },
        [&](TextureFile& texture_file) {
            // TODO!
        }
    );

    std::visit(asset_visitor, asset_file);
    return result;
}
