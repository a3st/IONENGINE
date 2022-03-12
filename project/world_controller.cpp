// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <project/world_controller.h>

#include <engine/asset_compiler.h>

using namespace project;

bool WorldController::initialize() {

    auto save_bytes_to_file = [&](std::filesystem::path const& path, std::vector<char8_t> const& data, std::ios::openmode const open_mode = std::ios::beg) {
        std::ofstream ofs(path, open_mode);
        if(!ofs.is_open()) {
            throw std::runtime_error("Can't open file!");
        }
        ofs.write(reinterpret_cast<char const*>(data.data()), data.size());
    };

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

    ionengine::AssetCompiler compiler;
    if(compiler.compile("shaders/shader_blob.shpk")) {
        std::cout << "Asset was compiled!" << std::endl;
    }

    //std::vector<char8_t> serialized_data;
    //compiler.serialize(serialized_data);
    //compiler.deserialize(serialized_data);
    // compiler.data() -> std::variant<MeshFile, TextureFile>*

    return true;
}