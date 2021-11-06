// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "shader.h"

using namespace lgfx;

Shader::Shader(Device* const device, const ShaderType type, const std::filesystem::path& path) : type_(type) {

    std::ifstream ifs(path, std::ios::binary);
    if(!ifs.is_open()) {
        throw std::runtime_error("Shader blob error loading");
    }

    ifs.seekg(0, std::ios::end);
    size_t size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    data_.resize(size);
    ifs.read(reinterpret_cast<char*>(data_.data()), data_.size());

    shader_.pShaderBytecode = data_.data();
    shader_.BytecodeLength = data_.size();
}

Shader::Shader(Shader&& rhs) noexcept {

    std::swap(type_, rhs.type_);
    std::swap(shader_, rhs.shader_);
    std::swap(data_, rhs.data_);
}

Shader& Shader::operator=(Shader&& rhs) noexcept {
    
    std::swap(type_, rhs.type_);
    std::swap(shader_, rhs.shader_);
    std::swap(data_, rhs.data_);
    return *this;
}