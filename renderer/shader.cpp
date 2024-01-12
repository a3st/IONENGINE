// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "shader.hpp"
#include "backend.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

ShaderReflect::ShaderReflect(std::string_view const shader_code) {

    std::cout << "Shader Reflect" << std::endl;

    std::regex type_space_regex("@group[(](\\d)[)] @binding[(](\\d)[)](?: |\\s*\n)var<(\\w+)[,]*(\\w+)*> (\\w+): ([\\w\\<\\>]+);");
    std::regex handle_space_regex("@group[(](\\d)[)] @binding[(](\\d)[)](?: |\\s*\n)var (\\w+): ([\\w\\<\\>]+);");

    {
        auto match_begin = std::regex_iterator<std::string_view::const_iterator>(shader_code.begin(), shader_code.end(), type_space_regex);
        auto match_end = std::regex_iterator<std::string_view::const_iterator>();

        for(auto it = match_begin; it != match_end; ++it) {
            auto match = *it;

            if(match.size() - 1 != MAX_GROUP_TYPE_SPACE) {
                std::cout << "Error parsing" << std::endl;
            }

            auto reflect_info = VariableReflectInfo {};
            reflect_info.name = match[5].str();
            
            auto variable_type = match[6].str();
            if(std::regex_search(variable_type, std::regex("mat\\w+<\\w+>"))) {
                reflect_info.variable_type = VariableType::Matrix;
            } else if(std::regex_search(variable_type, std::regex("vec\\w+<\\w+>"))) {
                reflect_info.variable_type = VariableType::Vector;
            } else if(std::regex_search(variable_type, std::regex("array\\w+<\\w+>"))) {
                reflect_info.variable_type = VariableType::Array;
            } else if(std::regex_search(variable_type, std::regex("sampler|texture_[2,3]d<\\w+>"))) {
                reflect_info.variable_type = VariableType::Template;
            } else {
                reflect_info.variable_type = VariableType::Struct;
            }

            reflect_info.group = std::stoi(match[1]);
            reflect_info.binding = std::stoi(match[2]);
            
            auto resource_type = match[3].str();
            if(resource_type.find("uniform") != std::string::npos) {
                reflect_info.resource_type = VariableResourceType::Uniform;
                reflect_info.access_type = VariableAccessType::Undefined;
            } else if(resource_type.find("storage") != std::string::npos) {
                reflect_info.resource_type = VariableResourceType::Storage;

                auto access_type = match[4].str();
                if(resource_type.find("read_write") != std::string::npos) {
                    reflect_info.access_type = VariableAccessType::ReadWrite;
                } else if(resource_type.find("write") != std::string::npos) {
                    reflect_info.access_type = VariableAccessType::Write;
                } else if(resource_type.find("read") != std::string::npos) {
                    reflect_info.access_type = VariableAccessType::Read;
                } else {
                    reflect_info.access_type = VariableAccessType::Undefined;
                }
            }

            bindings.try_emplace(reflect_info.group, std::unordered_map<uint32_t, VariableReflectInfo>());
            bindings.at(reflect_info.group).emplace(reflect_info.binding, reflect_info);
        }
    }

    {
        auto match_begin = std::regex_iterator<std::string_view::const_iterator>(shader_code.begin(), shader_code.end(), handle_space_regex);
        auto match_end = std::regex_iterator<std::string_view::const_iterator>();

        for(auto it = match_begin; it != match_end; ++it) {
            auto match = *it;

            if(match.size() - 1 != MAX_GROUP_HANDLE_SPACE) {
                std::cout << "Error parsing" << std::endl;
            }

            auto reflect_info = VariableReflectInfo {};
            reflect_info.name = match[3].str();

            auto variable_type = match[4].str();
            if(std::regex_search(variable_type, std::regex("mat\\w+<\\w+>"))) {
                reflect_info.variable_type = VariableType::Matrix;
            } else if(std::regex_search(variable_type, std::regex("vec\\w+<\\w+>"))) {
                reflect_info.variable_type = VariableType::Vector;
            } else if(std::regex_search(variable_type, std::regex("array\\w+<\\w+>"))) {
                reflect_info.variable_type = VariableType::Array;
            } else if(std::regex_search(variable_type, std::regex("sampler"))) {
                reflect_info.variable_type = VariableType::Template;
                reflect_info.resource_type = VariableResourceType::Sampler;
            } else if(std::regex_search(variable_type, std::regex("texture_[2,3]d<\\w+>"))) {
                reflect_info.variable_type = VariableType::Template;
                reflect_info.resource_type = VariableResourceType::Texture;
            } else {
                reflect_info.variable_type = VariableType::Struct;
            }

            reflect_info.group = std::stoi(match[1]);
            reflect_info.binding = std::stoi(match[2]);
        
            reflect_info.access_type = VariableAccessType::Undefined;

            bindings.try_emplace(reflect_info.group, std::unordered_map<uint32_t, VariableReflectInfo>());
            bindings.at(reflect_info.group).emplace(reflect_info.binding, reflect_info);
        }
    }
}

Shader::Shader(Backend& backend, std::string_view const shader_code) {

    ShaderReflect reflect(shader_code);
    {
        
    }

    {
        auto descriptor = wgpu::ShaderModuleDescriptor {};
        
        auto wgsl = wgpu::ShaderModuleWGSLDescriptor {};
        wgsl.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
        wgsl.code = shader_code.data();

        descriptor.nextInChain = &wgsl.chain;

        shader_module = backend.get_device().createShaderModule(descriptor);
    }

    {
        for(auto const& [group, group_info] : reflect.get_bindings()) {

            std::vector<wgpu::BindGroupLayoutEntry> entries;

            for(auto const& [binding, binding_info] : group_info) {
                auto entry = wgpu::BindGroupLayoutEntry {wgpu::Default};
                entry.binding = binding;
                entry.visibility = wgpu::ShaderStage::Fragment;
                
                switch(binding_info.resource_type) {
                    case VariableResourceType::Sampler: {
                        entry.sampler.type = wgpu::SamplerBindingType::Filtering;
                    } break;
                    case VariableResourceType::Texture: {
                        entry.texture.sampleType = wgpu::TextureSampleType::Float;
                    } break;
                    case VariableResourceType::Uniform: {
                        entry.buffer.type = wgpu::BufferBindingType::Uniform;
                    } break;
                    case VariableResourceType::Storage: {
                        entry.buffer.type = wgpu::BufferBindingType::Storage;
                    } break;
                }

                entries.emplace_back(entry);
            }

            auto descriptor = wgpu::BindGroupLayoutDescriptor {};
            descriptor.entryCount = entries.size();
            descriptor.entries = entries.data();

            bind_group_layout = backend.get_device().createBindGroupLayout(descriptor);
        }
    }
}