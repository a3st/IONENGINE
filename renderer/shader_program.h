// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <asset/technique.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class ShaderProgramError { };

enum class ShaderUniformType {
    CBuffer,
    Sampler2D,
    SBuffer,
    RWBuffer,
    RWTexture2D
};

template<ShaderUniformType Type>
struct ShaderUniformData { };

template<>
struct ShaderUniformData<ShaderUniformType::Sampler2D> { 
    uint32_t index;
};

template<>
struct ShaderUniformData<ShaderUniformType::RWTexture2D> { 
    uint32_t index;
};

template<>
struct ShaderUniformData<ShaderUniformType::CBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

template<>
struct ShaderUniformData<ShaderUniformType::SBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

template<>
struct ShaderUniformData<ShaderUniformType::RWBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

struct ShaderUniform {
    std::variant<
        ShaderUniformData<ShaderUniformType::Sampler2D>,
        ShaderUniformData<ShaderUniformType::CBuffer>,
        ShaderUniformData<ShaderUniformType::SBuffer>,
        ShaderUniformData<ShaderUniformType::RWBuffer>,
        ShaderUniformData<ShaderUniformType::RWTexture2D>
    > data;
};

struct ShaderProgram {
    std::vector<backend::Handle<backend::Shader>> shaders;
    backend::Handle<backend::DescriptorLayout> descriptor_layout;
    std::vector<backend::VertexInputDesc> attributes;
    std::unordered_map<std::string, ShaderUniform> uniforms;

    uint32_t location_uniform_by_name(std::string_view const name) const;

    lib::Expected<ShaderProgram, lib::Result<ShaderProgramError>> load_from_technique(backend::Device& device, asset::Technique const& technique);
};

template<>
struct GPUResourceDeleter<ShaderProgram> {
    void operator()(backend::Device& device, ShaderProgram const& shader_program) const {
        for(auto const& shader : shader_program.shaders) {
            device.delete_shader(shader);
        }
    }
};

backend::ShaderFlags constexpr get_shader_flags(asset::ShaderFlags const shader_flags);

backend::Format constexpr get_attribute_format(asset::ShaderDataType const data_type);

uint64_t constexpr get_shader_data_type_size(asset::ShaderDataType const data_type);

}
