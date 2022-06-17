// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/gpu_program.h>
#include <lib/math/matrix.h>
#include <xxhash/xxhash64.h>

using namespace ionengine;
using namespace ionengine::renderer;

lib::Expected<GPUProgram, lib::Result<GPUProgramError>> GPUProgram::load_from_shader(backend::Device& device, asset::Shader const& shader) {

    auto gpu_program = GPUProgram {};

    std::unordered_map<std::string, uint32_t> locations;
    std::unordered_map<backend::DescriptorType, uint32_t> registers_count;
    registers_count.insert({ backend::DescriptorType::ShaderResource, 0 });
    registers_count.insert({ backend::DescriptorType::ConstantBuffer, 0 });
    registers_count.insert({ backend::DescriptorType::UnorderedAccess, 0 });
    registers_count.insert({ backend::DescriptorType::Sampler, 0 });

    std::vector<backend::DescriptorLayoutBinding> bindings;
    uint32_t binding_index = 0;

    for(auto const& uniform : shader.uniforms) {

        ProgramDescriptor program_descriptor;

        auto uniform_visitor = make_visitor(
            [&](asset::ShaderUniformData<asset::ShaderUniformType::CBuffer> const& data) {

                auto descriptor_data = ProgramDescriptorData<ProgramDescriptorType::CBuffer> { .index = binding_index };
                calculate_uniform_offsets(data.data, descriptor_data.offsets);
                
                program_descriptor = ProgramDescriptor {
                    .data = std::move(descriptor_data)
                };

                bindings.emplace_back(binding_index, backend::DescriptorType::ConstantBuffer, get_shader_flags(uniform.visibility));
                ++binding_index;

                uint32_t const register_index = registers_count.at(backend::DescriptorType::ConstantBuffer);
                locations.insert({ uniform.name, register_index });

                ++registers_count.at(backend::DescriptorType::ConstantBuffer);
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::RWBuffer> const& data) {

                auto descriptor_data = ProgramDescriptorData<ProgramDescriptorType::RWBuffer> { .index = binding_index };
                calculate_uniform_offsets(data.data, descriptor_data.offsets);
                
                program_descriptor = ProgramDescriptor {
                    .data = std::move(descriptor_data)
                };

                bindings.emplace_back(binding_index, backend::DescriptorType::UnorderedAccess, get_shader_flags(uniform.visibility));
                ++binding_index;

                uint32_t const register_index = registers_count.at(backend::DescriptorType::UnorderedAccess);

                locations.insert({ uniform.name, register_index });
                ++registers_count.at(backend::DescriptorType::UnorderedAccess);
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::SBuffer> const& data) {

                auto descriptor_data = ProgramDescriptorData<ProgramDescriptorType::SBuffer> { .index = binding_index };
                calculate_uniform_offsets(data.data, descriptor_data.offsets);
                
                program_descriptor = ProgramDescriptor {
                    .data = std::move(descriptor_data)
                };

                bindings.emplace_back(binding_index, backend::DescriptorType::ShaderResource, get_shader_flags(uniform.visibility));
                ++binding_index;

                uint32_t const register_index = registers_count.at(backend::DescriptorType::ShaderResource);
                locations.insert({ uniform.name, register_index });

                ++registers_count.at(backend::DescriptorType::ShaderResource);
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::RWTexture2D> const& data) {

                auto descriptor_data = ProgramDescriptorData<ProgramDescriptorType::RWTexture2D> { .index = binding_index };
                
                program_descriptor = ProgramDescriptor {
                    .data = std::move(descriptor_data)
                };

                bindings.emplace_back(binding_index, backend::DescriptorType::UnorderedAccess, get_shader_flags(uniform.visibility));
                ++binding_index;

                uint32_t const register_index = registers_count.at(backend::DescriptorType::UnorderedAccess);
                locations.insert({ uniform.name, register_index });

                ++registers_count.at(backend::DescriptorType::UnorderedAccess);
            },
            [&](asset::ShaderUniformData<asset::ShaderUniformType::Sampler2D> const& data) {

                auto descriptor_data = ProgramDescriptorData<ProgramDescriptorType::Sampler2D> { .index = binding_index };
                
                program_descriptor = ProgramDescriptor {
                    .data = std::move(descriptor_data)
                };

                bindings.emplace_back(binding_index, backend::DescriptorType::ShaderResource, get_shader_flags(uniform.visibility));
                ++binding_index;

                bindings.emplace_back(binding_index, backend::DescriptorType::Sampler, get_shader_flags(uniform.visibility));
                ++binding_index;

                uint32_t const register_index = registers_count.at(backend::DescriptorType::ShaderResource);
                locations.insert({ uniform.name, register_index });

                ++registers_count.at(backend::DescriptorType::ShaderResource);
                ++registers_count.at(backend::DescriptorType::Sampler);
            }
        );

        std::visit(uniform_visitor, uniform.data);

        gpu_program.descriptors.insert({ uniform.name, std::move(program_descriptor) });
    }

    uint32_t stride = 0;

    for(size_t i = 0; i < shader.attributes.size(); ++i) {
        gpu_program.attributes.emplace_back(
            shader.attributes.at(i).semantic, 
            static_cast<uint32_t>(i), 
            get_shader_data_format(shader.attributes.at(i).type),
            0,
            stride
        );
        stride += static_cast<uint32_t>(get_shader_data_type_size(shader.attributes.at(i).type));
    }

    for(auto const& [stage_type, stage_source] : shader.stages) {

        std::string generated_code = "";

        for(auto& uniform : shader.uniforms) {

            if(uniform.visibility != stage_type && uniform.visibility != asset::ShaderType::All) {
                continue;
            }

            generated_code += generate_descriptor_code(uniform, locations.at(uniform.name));
        }

        generated_code += stage_source.source;

        gpu_program.stages.emplace_back(device.create_shader(generated_code, get_shader_flags(stage_type)));
    }

    gpu_program.descriptor_layout = device.create_descriptor_layout(bindings, false);

    return lib::make_expected<GPUProgram, lib::Result<GPUProgramError>>(std::move(gpu_program));
}

uint32_t GPUProgram::index_descriptor_by_name(std::string_view const name) const {
    auto it = descriptors.find(std::string(name));
    assert(it != descriptors.end() && "invalid descriptor index");

    uint32_t index;

    auto uniform_visitor = make_visitor(
        [&](ProgramDescriptorData<ProgramDescriptorType::Sampler2D> const& data) {
            index = data.index;
        },
        [&](ProgramDescriptorData<ProgramDescriptorType::CBuffer> const& data) {
            index = data.index;
        },
        [&](ProgramDescriptorData<ProgramDescriptorType::SBuffer> const& data) {
            index = data.index;
        },
        [&](ProgramDescriptorData<ProgramDescriptorType::RWBuffer> const& data) {
            index = data.index;
        },
        [&](ProgramDescriptorData<ProgramDescriptorType::RWTexture2D> const& data) {
            index = data.index;
        }
    );

    std::visit(uniform_visitor, it->second.data);
    return index;
}

void ionengine::renderer::calculate_uniform_offsets(std::span<asset::ShaderBufferData const> const data, std::unordered_map<std::string, uint64_t>& offsets) {
    uint64_t offset = 0;

    for(auto const& property : data) {
        offsets.insert({ property.name, offset });
        offset += get_shader_data_type_size(property.type);
    }
}

std::string ionengine::renderer::generate_descriptor_code(asset::ShaderUniform const& shader_uniform, uint32_t const location) {

    auto generate_struct_code = [](std::string_view const name, std::span<asset::ShaderBufferData const> const data) -> std::string {
        std::string generated_code = "struct";
        generated_code += std::format(" {} {{ ", name);
        for(auto& property : data) {
            generated_code += std::format("{} {}; ", get_shader_data_type_string(property.type), property.name);
        }
        generated_code += "};\n";
        return generated_code;
    };

    std::string generated_code = "";

    std::string const struct_name = std::format("{}_data", shader_uniform.name);

    auto uniform_visitor = make_visitor(
        [&](asset::ShaderUniformData<asset::ShaderUniformType::CBuffer> const& data) {
            generated_code += generate_struct_code(struct_name, data.data);
            generated_code += std::format("ConstantBuffer<{}> {} : register(b{});\n", struct_name, shader_uniform.name, location);
        },
        [&](asset::ShaderUniformData<asset::ShaderUniformType::SBuffer> const& data) {
            generated_code += generate_struct_code(struct_name, data.data);
            generated_code += std::format("StructuredBuffer<{}> {}s : register(t{});\n", struct_name, shader_uniform.name, location);
        },
        [&](asset::ShaderUniformData<asset::ShaderUniformType::RWBuffer> const& data) {
            generated_code += generate_struct_code(struct_name, data.data);
            generated_code += std::format("RWStructuredBuffer<{}> {}s : register(u{});\n", struct_name, shader_uniform.name, location);
        },
        [&](asset::ShaderUniformData<asset::ShaderUniformType::RWTexture2D> const& data) {
            generated_code += std::format("RWTexture2D<float> {}_texture : register(u{});\n", shader_uniform.name, location);
        },
        [&](asset::ShaderUniformData<asset::ShaderUniformType::Sampler2D> const& data) {
            generated_code += std::format("SamplerState {}_sampler : register(s{}); ", shader_uniform.name, location);
            generated_code += std::format("Texture2D {}_texture : register(t{});\n", shader_uniform.name, location);
        }
    );

    std::visit(uniform_visitor, shader_uniform.data);
    return generated_code;
}

backend::ShaderFlags constexpr ionengine::renderer::get_shader_flags(asset::ShaderType const shader_type) {
    switch(shader_type) {
        case asset::ShaderType::Vertex: return backend::ShaderFlags::Vertex;
        case asset::ShaderType::Pixel: return backend::ShaderFlags::Pixel;
        case asset::ShaderType::Geometry: return backend::ShaderFlags::Geometry;
        case asset::ShaderType::Domain: return backend::ShaderFlags::Domain;
        case asset::ShaderType::Hull: return backend::ShaderFlags::Hull;
        case asset::ShaderType::Compute: return backend::ShaderFlags::Compute;
        case asset::ShaderType::All: return backend::ShaderFlags::All;
        default: {
            assert(false && "invalid data type");
            return backend::ShaderFlags::All;
        }
    }
}

backend::Format constexpr ionengine::renderer::get_shader_data_format(asset::ShaderDataType const data_type) {
    switch(data_type) {
        case asset::ShaderDataType::Float: return backend::Format::R32;
        case asset::ShaderDataType::Float2: return backend::Format::RG32;
        case asset::ShaderDataType::Float3: return backend::Format::RGB32;
        case asset::ShaderDataType::Float4: return backend::Format::RGBA32;
        default: {
            assert(false && "invalid data type");
            return backend::Format::Unknown;
        }
    }
}

std::string constexpr ionengine::renderer::get_shader_data_type_string(asset::ShaderDataType const data_type) {
    switch(data_type) {
        case asset::ShaderDataType::Float4x4: return "float4x4";
        case asset::ShaderDataType::Float4: return "float4";
        case asset::ShaderDataType::Float3: return "float3";
        case asset::ShaderDataType::Float2: return "float2";
        case asset::ShaderDataType::Float: return "float";
        case asset::ShaderDataType::Uint: return "uint";
        case asset::ShaderDataType::Bool: return "bool";
        default: {
            assert(false && "invalid data type");
            return "float";
        }
    }
}

uint64_t constexpr ionengine::renderer::get_shader_data_type_size(asset::ShaderDataType const data_type) {
    switch(data_type) {
        case asset::ShaderDataType::Float: return sizeof(float);
        case asset::ShaderDataType::Float2: return sizeof(float) * 2;
        case asset::ShaderDataType::Float3: return sizeof(float) * 3;
        case asset::ShaderDataType::Float4: return sizeof(lib::math::Vector4f);
        case asset::ShaderDataType::Float4x4: return sizeof(lib::math::Matrixf);
        case asset::ShaderDataType::Uint: return sizeof(uint32_t);
        case asset::ShaderDataType::Bool: return sizeof(bool);
        default: {
            assert(false && "invalid data type");
            return 0;
        }
    }
}
