// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

std::vector<byte> read_shader_code(const std::filesystem::path& shader_path) {

    std::vector<byte> shader_code;

    std::ifstream file(shader_path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(format<std::string>("Shader cannot be opened '{}'", shader_path));
    }

    usize file_size = static_cast<usize>(file.tellg());
    shader_code.resize(file_size);

    file.seekg(std::ios::beg);
    file.read(reinterpret_cast<char*>(shader_code.data()), file_size);
    return std::move(shader_code);
}

}