// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "shader.hpp"
#include "context.hpp"
#include "core/exception.hpp"
#include <xxhash/xxhash64.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderReflect::ShaderReflect(std::string_view const shader_code) {

    std::string only_code(shader_code.data(), shader_code.size());
    {
        std::regex pattern("\\/\\/.+\\n?");
        only_code = std::regex_replace(only_code, pattern, "");
    }

    {
        std::regex pattern("@group[(](\\d)[)] @binding[(](\\d)[)](?: |\\s*\n)var<(\\w+)[,]*(\\w+)*> (\\w+): ([\\w\\<\\>]+);");
        auto match_begin = std::sregex_iterator(only_code.begin(), only_code.end(), pattern);
        auto match_end = std::sregex_iterator();

        for(auto it = match_begin; it != match_end; ++it) {
            auto match = *it;

            if(match.size() - 1 != MAX_GROUP_TYPE_SPACE) {
                return;
            }

            auto reflect_info = VariableReflectInfo {};
            reflect_info.name = match[5].str();
            
            parse_variable_type(match[6].str(), reflect_info);

            reflect_info.group = std::stoi(match[1]);
            reflect_info.binding = std::stoi(match[2]);
            
            parse_resource_type({ match[3].str(), match[4].str() }, reflect_info);

            bindings.try_emplace(reflect_info.group, std::unordered_map<uint32_t, VariableReflectInfo>());
            bindings.at(reflect_info.group).emplace(reflect_info.binding, reflect_info);
        }
    }

    {
        std::regex pattern("@group[(](\\d)[)] @binding[(](\\d)[)](?: |\\s*\n)var (\\w+): ([\\w\\<\\>]+);");
        auto match_begin = std::sregex_iterator(only_code.begin(), only_code.end(), pattern);
        auto match_end = std::sregex_iterator();

        for(auto it = match_begin; it != match_end; ++it) {
            auto match = *it;

            if(match.size() - 1 != MAX_GROUP_HANDLE_SPACE) {
                return;
            }

            auto reflect_info = VariableReflectInfo {};
            reflect_info.name = match[3].str();

            parse_variable_type(match[4].str(), reflect_info);

            reflect_info.group = std::stoi(match[1]);
            reflect_info.binding = std::stoi(match[2]);
        
            reflect_info.access_type = VariableAccessType::Undefined;

            bindings.try_emplace(reflect_info.group, std::unordered_map<uint32_t, VariableReflectInfo>());
            bindings.at(reflect_info.group).emplace(reflect_info.binding, reflect_info);
        }
    }

    {
        std::regex pattern("@(fragment|vertex|compute)");
        auto match_begin = std::sregex_iterator(only_code.begin(), only_code.end(), pattern);
        auto match_end = std::sregex_iterator();

        for(auto it = match_begin; it != match_end; ++it) {
            auto match = *it;
            
            if(match.size() - 1 != 1) {
                return;
            }

            auto shader_stage = match[1].str();

            if(shader_stage.find("vertex") != std::string::npos) {
                stages |= wgpu::ShaderStage::Vertex;
            } else if(shader_stage.find("fragment") != std::string::npos) {
                stages |= wgpu::ShaderStage::Fragment;
            } else if(shader_stage.find("compute") != std::string::npos) {
                stages |= wgpu::ShaderStage::Compute;
            }
        }
    }
}

auto ShaderReflect::get_format_by_string(std::string_view const format) -> VariableFormat {
    
    VariableFormat ret = VariableFormat::Undefined;
    
    if(format.find("f32") != std::string_view::npos) {
        ret = VariableFormat::Float32;
    } else if(format.find("i32") != std::string_view::npos) {
        ret = VariableFormat::Sint32;
    } else if(format.find("u32") != std::string_view::npos) {
        ret = VariableFormat::Uint32;
    }

    return ret;
}

auto ShaderReflect::get_access_by_string(std::string_view const access) -> VariableAccessType {

    VariableAccessType ret = VariableAccessType::Undefined;

    if(access.find("read_write") != std::string::npos) {
        ret = VariableAccessType::ReadWrite;
    } else if(access.find("write") != std::string::npos) {
        ret = VariableAccessType::Write;
    } else if(access.find("read") != std::string::npos) {
        ret = VariableAccessType::Read;
    } else {
        ret = VariableAccessType::Undefined;
    }

    return ret;
}

auto ShaderReflect::parse_variable_type(std::string const& source, VariableReflectInfo& reflect_info) -> void {

    std::smatch variable_match;

    if(std::regex_match(source, variable_match, std::regex("mat[2,3,4]x[2,3,4]<(\\w+)>"))) {
        reflect_info.variable_info = VariableInfo {
            .variable_type = VariableType::Matrix,
            .format = get_format_by_string(variable_match[1].str())
        };
    } else if(std::regex_match(source, variable_match, std::regex("vec[2,3,4]x[2,3,4]<(\\w+)>"))) {
        reflect_info.variable_info = VariableInfo {
            .variable_type = VariableType::Vector,
            .format = get_format_by_string(variable_match[1].str())
        };
    } else if(std::regex_match(source, variable_match, std::regex("array<(\\w+),\\d+>"))) {
        reflect_info.variable_info = VariableInfo {
            .variable_type = VariableType::Array,
            .format = get_format_by_string(variable_match[1].str())
        };
    } else if(std::regex_match(source, variable_match, std::regex("sampler"))) {
        reflect_info.variable_info = VariableInfo {
            .variable_type = VariableType::Sampler,
            .format = VariableFormat::Undefined
        };

        reflect_info.resource_type = VariableResourceType::Sampler;
    } else if(std::regex_match(source, variable_match, std::regex("sampler_comparison"))) {
        reflect_info.variable_info = VariableInfo {
            .variable_type = VariableType::SamplerComparison,
            .format = VariableFormat::Undefined
        };

        reflect_info.resource_type = VariableResourceType::Sampler;
    } else if(std::regex_match(source, variable_match, std::regex("texture_((?:[2]d_|cube_)array|cube|[1,2,3]d)<(\\w+)>"))) {
        auto texture_dimension = variable_match[1].str();
        auto texture_format = variable_match[2].str();

        if(texture_dimension.find("1d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::Texture1D,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_dimension.find("2d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::Texture2D,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_dimension.find("2d_array")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::Texture2DArray,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_dimension.find("3d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::Texture3D,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_dimension.find("cube")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureCube,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_dimension.find("cube_array")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureCubeArray,
                .format = get_format_by_string(texture_format)
            };
        }

        reflect_info.resource_type = VariableResourceType::Texture;
    } else if(std::regex_match(source, variable_match, std::regex("texture_(multisampled_2d<(\\w+)>|depth_multisampled_2d)"))) {
        auto texture_type = variable_match[1].str();
        auto texture_format = variable_match[2].str();

        if(texture_type.find("multisampled_2d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureMultisampled2D,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_type.find("depth_multisampled_2d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureDepthMultisampled2D,
                .format = VariableFormat::Undefined
            };
        }

        reflect_info.resource_type = VariableResourceType::Texture;
    } else if(std::regex_match(source, variable_match, std::regex("texture_depth_([2]d_array|[2]d|cube_array|cube)"))) {
        auto texture_type = variable_match[1].str();

        if(texture_type.find("2d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureDepth2D,
                .format = VariableFormat::Undefined
            };
        } else if(texture_type.find("2d_array")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureDepth2DArray,
                .format = VariableFormat::Undefined
            };
        } else if(texture_type.find("cube")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureDepthCube,
                .format = VariableFormat::Undefined
            };
        } else if(texture_type.find("cube_array")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureDepthCubeArray,
                .format = VariableFormat::Undefined
            };
        }

        reflect_info.resource_type = VariableResourceType::Texture;
    } else if(std::regex_match(source, variable_match, std::regex("texture_storage_([2]d_array|[1,2,3]d)<(\\w+),(\\w+)>"))) {
        auto texture_type = variable_match[1].str();
        auto texture_format = variable_match[2].str();

        if(texture_type.find("1d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureStorage1D,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_type.find("2d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureStorage2D,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_type.find("2d_array")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureStorage2DArray,
                .format = get_format_by_string(texture_format)
            };
        } else if(texture_type.find("3d")) {
            reflect_info.variable_info = VariableInfo {
                .variable_type = VariableType::TextureStorage3D,
                .format = get_format_by_string(texture_format)
            };
        }

        reflect_info.resource_type = VariableResourceType::StorageTexture;
        reflect_info.access_type = get_access_by_string(variable_match[3].str());
    } else {
        reflect_info.variable_info = VariableInfo {
            .variable_type = VariableType::Struct,
            .format = VariableFormat::Undefined
        };
    }
}

auto ShaderReflect::parse_resource_type(std::tuple<std::string, std::string> const sources, VariableReflectInfo& reflect_info) -> void {

    if(std::get<0>(sources).find("uniform") != std::string::npos) {
        reflect_info.resource_type = VariableResourceType::Uniform;
        reflect_info.access_type = VariableAccessType::Undefined;
    } else if(std::get<0>(sources).find("storage") != std::string::npos) {
        reflect_info.resource_type = VariableResourceType::Storage;
        reflect_info.access_type = get_access_by_string(std::get<1>(sources));
    }
}

Shader::Shader(Context& context, std::string_view const shader_code) {

    ShaderReflect reflect(shader_code);

    {
        auto descriptor = wgpu::ShaderModuleDescriptor {};
        
        auto wgsl = wgpu::ShaderModuleWGSLDescriptor {};
        wgsl.chain.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
        wgsl.code = shader_code.data();

        descriptor.nextInChain = &wgsl.chain;

        shader_module = context.get_device().createShaderModule(descriptor);
    }

    {
        for(auto const& [group, group_info] : reflect.get_bindings()) {

            std::vector<wgpu::BindGroupLayoutEntry> entries;

            for(auto const& [binding, binding_info] : group_info) {
                auto entry = wgpu::BindGroupLayoutEntry {wgpu::Default};
                entry.binding = binding;
                
                switch(binding_info.resource_type) {
                    case VariableResourceType::Sampler: {
                        if(binding_info.variable_info.variable_type == VariableType::Sampler) {
                            entry.sampler.type = wgpu::SamplerBindingType::Filtering;
                        } else if(binding_info.variable_info.variable_type == VariableType::SamplerComparison) {
                            entry.sampler.type = wgpu::SamplerBindingType::Comparison;
                        } else {
                            entry.sampler.type = wgpu::SamplerBindingType::Undefined;
                        }

                        entry.visibility = wgpu::ShaderStage::Fragment;
                    } break;
                    case VariableResourceType::Texture: {
                        switch(binding_info.variable_info.format) {
                            case VariableFormat::Float32: {
                                entry.texture.sampleType = wgpu::TextureSampleType::Float;
                            } break;
                            case VariableFormat::Sint32: {
                                entry.texture.sampleType = wgpu::TextureSampleType::Sint;
                            } break;
                            case VariableFormat::Uint32: {
                                entry.texture.sampleType = wgpu::TextureSampleType::Uint;
                            } break;
                            default: {
                                entry.texture.sampleType = wgpu::TextureSampleType::Depth;
                            } break;
                        }
                        
                        switch(binding_info.variable_info.variable_type) {
                            case VariableType::Texture1D: {
                                entry.texture.viewDimension = wgpu::TextureViewDimension::_1D;
                            } break;
                            case VariableType::TextureDepth2D:
                            case VariableType::Texture2D: {
                                entry.texture.viewDimension = wgpu::TextureViewDimension::_2D;
                            } break;
                            case VariableType::Texture3D: {
                                entry.texture.viewDimension = wgpu::TextureViewDimension::_3D;
                            } break;
                            case VariableType::TextureDepthCube:
                            case VariableType::TextureCube: {
                                entry.texture.viewDimension = wgpu::TextureViewDimension::Cube;
                            } break;
                            case VariableType::TextureDepthCubeArray:
                            case VariableType::TextureCubeArray: {
                                entry.texture.viewDimension = wgpu::TextureViewDimension::CubeArray;
                            } break;
                            case VariableType::TextureDepthMultisampled2D:
                            case VariableType::TextureMultisampled2D: {
                                entry.texture.viewDimension = wgpu::TextureViewDimension::_2D;
                                entry.texture.multisampled = true;
                            } break;
                        }

                        entry.visibility = wgpu::ShaderStage::Fragment;
                    } break;
                    case VariableResourceType::StorageTexture: {
                        switch(binding_info.variable_info.format) {
                            case VariableFormat::Float32: {
                                entry.storageTexture.format = wgpu::TextureFormat::RGBA8Unorm;
                            } break;
                            case VariableFormat::Sint32: {
                                entry.storageTexture.format = wgpu::TextureFormat::RGBA8Snorm;
                            } break;
                            case VariableFormat::Uint32: {
                                entry.storageTexture.format = wgpu::TextureFormat::RGBA8Uint;
                            } break;
                        }

                        switch(binding_info.variable_info.variable_type) {
                            case VariableType::TextureStorage1D: {
                                entry.storageTexture.viewDimension = wgpu::TextureViewDimension::_1D;
                            } break;
                            case VariableType::TextureStorage2D: {
                                entry.storageTexture.viewDimension = wgpu::TextureViewDimension::_2D;
                            } break;
                            case VariableType::TextureStorage2DArray: {
                                entry.storageTexture.viewDimension = wgpu::TextureViewDimension::_2DArray;
                            } break;
                            case VariableType::TextureStorage3D: {
                                entry.storageTexture.viewDimension = wgpu::TextureViewDimension::_3D;
                            } break;
                        }

                        switch(binding_info.access_type) {
                            case VariableAccessType::Write: {
                                entry.storageTexture.access = wgpu::StorageTextureAccess::WriteOnly;
                            } break;
                            default: {
                                entry.storageTexture.access = wgpu::StorageTextureAccess::Undefined;
                            }
                        }

                        if(reflect.get_stages() & wgpu::ShaderStage::Compute) {
                            entry.visibility = wgpu::ShaderStage::Compute;
                        } else {
                            entry.visibility = wgpu::ShaderStage::Fragment;
                        }
                    } break;
                    case VariableResourceType::Uniform: {
                        entry.buffer.type = wgpu::BufferBindingType::Uniform;

                        entry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
                    } break;
                    case VariableResourceType::Storage: {
                        if(binding_info.access_type == VariableAccessType::Read) {
                            entry.buffer.type = wgpu::BufferBindingType::ReadOnlyStorage;

                            if(reflect.get_stages() & wgpu::ShaderStage::Compute) {
                                entry.visibility = wgpu::ShaderStage::Compute;
                            } else {
                                entry.visibility = wgpu::ShaderStage::Vertex | wgpu::ShaderStage::Fragment;
                            }
                        } else {
                            entry.buffer.type = wgpu::BufferBindingType::Storage;

                            if(reflect.get_stages() & wgpu::ShaderStage::Compute) {
                                entry.visibility = wgpu::ShaderStage::Compute;
                            } else {
                                entry.visibility = wgpu::ShaderStage::Fragment;
                            }
                        }
                    } break;
                }

                entries.emplace_back(entry);
            }

            auto descriptor = wgpu::BindGroupLayoutDescriptor {};
            descriptor.entryCount = entries.size();
            descriptor.entries = entries.data();

            bind_group_layout = context.get_device().createBindGroupLayout(descriptor);
        }
    }
}

ShaderCache::ShaderCache(Context& context) : context(&context) {

}

auto ShaderCache::get(ShaderData const& data) -> core::ref_ptr<Shader> {

    uint64_t const hash = XXHash64::hash(data.shader_name.data(), data.shader_name.size(), 0);
    auto entry = entries.find(hash);

    if(entry != entries.end()) {
        return entry->second;
    } else {
        if(data.shader_code.empty()) {
            return nullptr;
        } else {
            auto shader = core::make_ref<Shader>(*context, data.shader_code);
            entries.emplace(hash, shader);
            return shader;
        }
    }
}