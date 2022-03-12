// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <engine/hlsv_loader.h>

#include <lib/algorithm.h>

using namespace ionengine;

bool HLSVLoader::parse(std::span<char8_t const> const data) {

    auto hlsv_spirv_file = hlsl_spirv_package::HLSL_SPIRV_File {};

    std::span<char8_t> buffer;
    uint64_t offset = 0;

    if(read_bytes(data, offset, buffer, sizeof(hlsl_spirv_package::HLSL_SPIRV_Magic)) > 0) {
        std::memcpy(&hlsv_spirv_file.magic, buffer.data(), buffer.size());
    } else {
        std::cerr << "[Error] HLSVLoader: Data cannot be read because it is corrupted" << std::endl;
        return false;
    }

    if(hlsv_spirv_file.magic != hlsl_spirv_package::HLSL_SPIRV_Magic) {
        std::cerr << "[Error] HLSVLoader: Data cannot be read because it is in a different format" << std::endl;
        return false;
    }

    if(read_bytes(data, offset, buffer, sizeof(hlsl_spirv_package::HLSL_SPIRV_Header)) > 0) {
        std::memcpy(&hlsv_spirv_file.header, buffer.data(), buffer.size());
    } else {
        std::cerr << "[Error] HLSVLoader: Data cannot be read because it is corrupted" << std::endl;
        return false;
    }

    hlsv_spirv_file.shaders = std::make_unique<hlsl_spirv_package::HLSL_SPIRV_Shader[]>(hlsv_spirv_file.header.count);
    size_t const shader_size = hlsv_spirv_file.header.count * sizeof(hlsl_spirv_package::HLSL_SPIRV_Shader);

    if(read_bytes(data, offset, buffer, shader_size) > 0) {
        std::memcpy(hlsv_spirv_file.shaders.get(), buffer.data(), buffer.size());
    } else {
        std::cerr << "[Error] HLSVLoader: Data cannot be read opened because it is corrupted" << std::endl;
        return false;
    }

    size_t const data_size = data.size() - offset;
    hlsv_spirv_file.data = std::make_unique<char8_t[]>(data_size);

    if(read_bytes(data, offset, buffer, data_size) > 0) {
        std::memcpy(hlsv_spirv_file.data.get(), buffer.data(), buffer.size());
    } else {
        std::cerr << "[Error] HLSVLoader: Data cannot be read because it is corrupted" << std::endl;
        return false;
    }

    _shaders.reserve(hlsv_spirv_file.header.count);

    offset = sizeof(hlsl_spirv_package::HLSL_SPIRV_Magic) + sizeof(hlsl_spirv_package::HLSL_SPIRV_Header) + shader_size;

    for(uint32_t i = 0; i < hlsv_spirv_file.header.count; ++i) {

        auto hlsv_shader = HLSVShader {};
        hlsv_shader.name = std::u8string(hlsv_spirv_file.shaders[i].name);
        hlsv_shader.flags = static_cast<HLSVFlags>(hlsv_spirv_file.shaders[i].flags);
        
        hlsv_shader.data.resize(hlsv_spirv_file.shaders[i].size);
        std::memcpy(hlsv_shader.data.data(), data.data() + offset, hlsv_spirv_file.shaders[i].size);

        _shaders.emplace_back(hlsv_shader);
    }

    return true;
}
