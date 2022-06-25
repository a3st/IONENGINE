// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <asset/shader.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class GPUProgramError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

enum class ProgramDescriptorType {
    CBuffer,
    Sampler2D,
    SamplerCube,
    SBuffer,
    RWBuffer,
    RWTexture2D
};

template<ProgramDescriptorType Type>
struct ProgramDescriptorData { };

template<>
struct ProgramDescriptorData<ProgramDescriptorType::Sampler2D> { 
    uint32_t index;
};

template<>
struct ProgramDescriptorData<ProgramDescriptorType::SamplerCube> { 
    uint32_t index;
};


template<>
struct ProgramDescriptorData<ProgramDescriptorType::RWTexture2D> { 
    uint32_t index;
};

template<>
struct ProgramDescriptorData<ProgramDescriptorType::CBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

template<>
struct ProgramDescriptorData<ProgramDescriptorType::SBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

template<>
struct ProgramDescriptorData<ProgramDescriptorType::RWBuffer> {
    uint32_t index;
    std::unordered_map<std::string, uint64_t> offsets; 
};

#ifndef DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST
#define DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST(Name, Type) \
ProgramDescriptorData<Type> const& Name() const { \
    return std::get<ProgramDescriptorData<Type>>(data); \
}
#endif

struct ProgramDescriptor {
    std::variant<
        ProgramDescriptorData<ProgramDescriptorType::Sampler2D>,
        ProgramDescriptorData<ProgramDescriptorType::SamplerCube>,
        ProgramDescriptorData<ProgramDescriptorType::CBuffer>,
        ProgramDescriptorData<ProgramDescriptorType::SBuffer>,
        ProgramDescriptorData<ProgramDescriptorType::RWBuffer>,
        ProgramDescriptorData<ProgramDescriptorType::RWTexture2D>
    > data;

    DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST(as_sampler2D, ProgramDescriptorType::Sampler2D)
    DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST(as_samplerCube, ProgramDescriptorType::SamplerCube)
    DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST(as_cbuffer, ProgramDescriptorType::CBuffer)
    DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST(as_sbuffer, ProgramDescriptorType::SBuffer)
    DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST(as_rwbuffer, ProgramDescriptorType::RWBuffer)
    DECLARE_PROGRAM_DESCRIPTOR_CONST_CAST(as_rwtexture2D, ProgramDescriptorType::RWTexture2D)
};

struct GPUProgram {
    std::vector<backend::Handle<backend::Shader>> stages;
    backend::Handle<backend::DescriptorLayout> descriptor_layout;
    std::vector<backend::VertexInputDesc> attributes;
    std::unordered_map<std::string, ProgramDescriptor> descriptors;

    uint32_t index_descriptor_by_name(std::string_view const name) const;

    static lib::Expected<GPUProgram, lib::Result<GPUProgramError>> load_from_shader(backend::Device& device, asset::ShaderVariant const& shader_variant);
};

template<>
struct GPUResourceDeleter<GPUProgram> {
    void operator()(backend::Device& device, GPUProgram const& program) const {
        for(auto const& stage : program.stages) {
            device.delete_shader(stage);
        }
    }
};

backend::ShaderFlags constexpr get_shader_flags(asset::ShaderType const shader_type);

backend::Format constexpr get_shader_data_format(asset::ShaderDataType const data_type);

uint64_t constexpr get_shader_data_type_size(asset::ShaderDataType const data_type);

std::string constexpr get_shader_data_type_string(asset::ShaderDataType const data_type);

std::string generate_descriptor_code(asset::ShaderUniform const& shader_uniform, uint32_t const location);

void calculate_uniform_offsets(std::span<asset::ShaderBufferData const> const data, std::unordered_map<std::string, uint64_t>& offsets);

}
