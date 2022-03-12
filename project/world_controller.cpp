// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <project/world_controller.h>

#include <engine/asset_compiler.h>

#include <lib/algorithm.h>

using namespace project;

bool WorldController::initialize() {

    ionengine::AssetFile asset_file;

    std::filesystem::path file_path = "shader_package.hlsv";

    ionengine::AssetCompiler compiler;
    if(compiler.compile(file_path, asset_file)) {
        std::cout << std::format("Asset '{}' is compiled to ShaderFile (IONENGINE) Asset", file_path.string()) << std::endl;
    }

    std::vector<char8_t> serialized_data;
    compiler.serialize(serialized_data, asset_file);

    ionengine::save_bytes_to_file("shader_package.asset", serialized_data, std::ios::binary);
    
    

    return true;
}